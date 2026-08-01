#include "../../include/engine/MatchingEngine.hpp"

#include <immintrin.h>

void MatchingEngine::submit(Order* order) {
        if(order == nullptr) return;
        m_order_queue.push(order);
}

bool MatchingEngine::cancel(u64 order_id) {     // Will revisit after wiring up Network Layer. Needs to be thread safe.
        return m_book.cancel(order_id);         // Route cancel request through SPSC queue rather than calling m_book.cancel() directly
}                                               // Push a ClientMessage variant onto the queue and let the worker thread handle both order submission and cancellations sequentially

void MatchingEngine::start() {
        m_running = true;
        m_worker = std::thread(&MatchingEngine::process_orders, this);
}

void MatchingEngine::shutdown() {
        m_running = false;
        m_worker.join();
}

bool MatchingEngine::pop_result(Trade& out) {
        return m_result_queue.pop(out);
}

const OrderBook& MatchingEngine::book() const {
        return m_book;
}

const std::vector<Trade>& MatchingEngine::trades() const {
        return m_trade_log;
}

void MatchingEngine::process_orders() {
        while(m_running) {
                Order* order;
                if(!m_order_queue.pop(order)) {
                        _mm_pause();
                        continue;
                }

                if(order->m_type == OrderType::LIMIT && !order->m_price) {
                        order->m_status = OrderStatus::REJECTED;
                        continue;
                }

                if(order->m_quantity_remaining == 0) {
                        order->m_status = OrderStatus::REJECTED;
                        continue;
                }

                auto process = [&](auto& opposing_side) {
                        // FOK Check
                        if(order->m_time_in_force == TimeInForce::FOK) {
                                u64 sum = 0;
                                for(const auto& [price, level] : opposing_side) {
                                        if(order->m_side == OrderSide::BID && order->m_price && price > *order->m_price) break;
                                        if(order->m_side == OrderSide::ASK && order->m_price && price < *order->m_price) break;
                                        sum += level.total_quantity();
                                }

                                if(sum < order->m_quantity_remaining) {
                                        order->m_status = OrderStatus::REJECTED;
                                        return;
                                }
                        }

                        // Matching loop
                        std::vector<Price> to_erase;
                        for(auto& [price, level] : opposing_side) {
                                if(order->m_side == OrderSide::BID && order->m_price && price > *order->m_price) break;
                                if(order->m_side == OrderSide::ASK && order->m_price && price < *order->m_price) break;

                                while(!level.is_empty() && order->m_quantity_remaining > 0) {
                                        if(level.front()->m_status == OrderStatus::CANCELLED) {
                                                level.pop();
                                                continue;
                                        }

                                        u64 fill_quantity = std::min(order->m_quantity_remaining, level.front()->m_quantity_remaining);
                                        u64 bid_id = (order->m_side == OrderSide::BID) ? order->m_order_id : level.front()->m_order_id;
                                        u64 ask_id = (order->m_side == OrderSide::ASK) ? order->m_order_id : level.front()->m_order_id;
                                        Trade trade(m_next_trade_id++, bid_id, ask_id, order->m_symbol, price, fill_quantity);

                                        m_trade_log.push_back(trade);
                                        m_result_queue.push(trade);

                                        order->m_quantity_remaining -= fill_quantity;
                                        level.front()->m_quantity_remaining -= fill_quantity;
                                        order->m_status = (order->m_quantity_remaining == 0) ? OrderStatus::FILLED : OrderStatus::PARTIALLY_FILLED;
                                        level.front()->m_status = (level.front()->m_quantity_remaining == 0) ? OrderStatus::FILLED : OrderStatus::PARTIALLY_FILLED;

                                        if(level.front()->m_status == OrderStatus::FILLED) level.pop();
                                        if(order->m_status == OrderStatus::FILLED) break;
                                }

                                if(level.is_empty()) to_erase.push_back(price);
                                if(order->m_quantity_remaining == 0) break;
                        }

                        for(Price p : to_erase) opposing_side.erase(p);

                        if(order->m_quantity_remaining > 0) {
                                switch(order->m_time_in_force) {
                                        case TimeInForce::GTC: m_book.insert(order); break;
                                        case TimeInForce::IOC: order->m_status = OrderStatus::CANCELLED; break;
                                        case TimeInForce::FOK: break;
                                        case TimeInForce::GTD: m_book.insert(order); break;
                                }
                        }
                };

                if(order->m_side == OrderSide::BID) process(m_book.asks());
                else process(m_book.bids());
        }
}
