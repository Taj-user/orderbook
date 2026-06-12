#include "../../include/core/MatchingEngine.h"

void MatchingEngine::submit(Order* order) {
    // Validation
    if(order == nullptr) return;
    if((order->order_type_ == OrderType::LIMIT && order->price_ == std::nullopt) || (order->order_type_ == OrderType::MARKET && order->price_ != std::nullopt) || order->quantity_original_ <= 0) {
        order->status_ = OrderStatus::REJECTED;
        return;
    }

    auto process = [&](auto& opposing_side) {
        // FOK check
        if(order->time_in_force_ == TimeInForce::FOK) {
            u64 sum = 0;
            for(const auto& [price, level] : opposing_side) {
                if(order->side_ == Side::BUY && order->price_ && price > *order->price_) break;
                if(order->side_ == Side::SELL && order->price_ && price < *order->price_) break;
                sum += level.totalQuantity();
            }
            if(sum < order->quantity_remaining_) {
                order->status_ = OrderStatus::REJECTED;
                return;
            }
        }
        std::vector<i64> to_erase;
        // Matching loop
        for(auto& [price, level] : opposing_side) {
            if(order->side_ == Side::BUY && order->price_ && price > *order->price_) break;
            if(order->side_ == Side::SELL && order->price_ && price < *order->price_) break;
            while(!level.isEmpty() && order->quantity_remaining_ > 0) {
                if(level.front()->status_ == OrderStatus::CANCELLED) {
                    level.pop();
                    continue;
                }
                u64 fill_qty = std::min(order->quantity_remaining_, level.front()->quantity_remaining_);
                u64 buy_id = (order->side_ == Side::BUY) ? order->order_id_ : level.front()->order_id_;
                u64 sell_id = (order->side_ == Side::SELL) ? order->order_id_ : level.front()->order_id_;
                trade_log_.emplace_back(next_trade_id_++, buy_id, sell_id, order->symbol_, price, fill_qty);

                order->quantity_remaining_ -= fill_qty;
                level.front()->quantity_remaining_ -= fill_qty;
                order->status_ = (order->quantity_remaining_ == 0) ? OrderStatus::FILLED : OrderStatus::PARTIALLY_FILLED;
                level.front()->status_ = (level.front()->quantity_remaining_ == 0) ? OrderStatus::FILLED : OrderStatus::PARTIALLY_FILLED;

                if(level.front()->status_ == OrderStatus::FILLED) level.pop();
                if(order->status_ == OrderStatus::FILLED) break;
            }
            if(level.isEmpty()) to_erase.push_back(price);
            if(order->quantity_remaining_ == 0) break;
        }
        for(i64 p : to_erase) opposing_side.erase(p);
        if(order->quantity_remaining_ > 0) {
            switch(order->time_in_force_) {
                case TimeInForce::GTC: book_.insert(order); break;
                case TimeInForce::IOC: order->status_ = OrderStatus::CANCELLED; break;
                case TimeInForce::FOK: break;
                case TimeInForce::GTD: book_.insert(order); break;
            }
        }
    };
    if(order->side_ == Side::BUY) process(book_.asks());
    else process(book_.bids());
}

// TODO: cancel method
