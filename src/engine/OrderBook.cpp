#include "../../include/engine/OrderBook.hpp"

void OrderBook::insert(Order* order) {
        if(order == nullptr) return;
        if(order->m_price) {
                if(order->m_side == OrderSide::BID) {
                        auto& level = m_bids.try_emplace(*order->m_price, *order->m_price).first->second;
                        level.add(order);
                }
                else {
                        auto& level = m_asks.try_emplace(*order->m_price, *order->m_price).first->second;
                        level.add(order);
                }
                m_order_lookup[order->m_order_id] = order;
        }
}

bool OrderBook::cancel(u64 order_id) {
        auto it = m_order_lookup.find(order_id);
        if(it != m_order_lookup.end()) {
                if(it->second->m_status == OrderStatus::OPEN || it->second->m_status == OrderStatus::PARTIALLY_FILLED) {
                        it->second->m_status = OrderStatus::CANCELLED;
                        m_order_lookup.erase(it);
                        return true;
                }
        }
        return false;
}

std::optional<Price> OrderBook::best_bid() const {
        if(m_bids.empty()) return std::nullopt;
        return m_bids.begin()->first;
}

std::optional<Price> OrderBook::best_ask() const {
        if(m_asks.empty()) return std::nullopt;
        return m_asks.begin()->first;
}

const std::map<Price, PriceLevel, std::greater<Price>>& OrderBook::bids() const {
        return m_bids;
}

const std::map<Price, PriceLevel>& OrderBook::asks() const {
        return m_asks;
}
