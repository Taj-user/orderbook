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
        // Matching loop
        for(auto& [price, level] : opposing_side) {
            
        }
    };
    if(order->side_ == Side::BUY) process(book_.asks());
    else process(book_.bids());
}
