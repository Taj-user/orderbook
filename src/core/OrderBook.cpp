#include "../../include/core/OrderBook.h"

void OrderBook::insert(Order* order) {
    if(order == nullptr) return;

    if(order->price_) {
        if(order->side_ == Side::BUY) {
            auto& level = bids_.try_emplace(*order->price_, *order->price_).first->second;
            level.add(order);
        }
        else {
            auto& level = asks_.try_emplace(*order->price_, *order->price_).first->second;
            level.add(order);
        }
        order_map_[order->order_id_] = order;
    }
}

bool OrderBook::cancel(u64 order_id) {
    auto it = order_map_.find(order_id);

    if(it != order_map_.end()) {
        if(it->second->status_ == OrderStatus::OPEN || it->second->status_ == OrderStatus::PARTIALLY_FILLED) {
            it->second->status_ = OrderStatus::CANCELLED;
            order_map_.erase(it);

            return true;
        }
    }
    return false;
}

std::optional<i64> OrderBook::bestBid() const {
   if(bids_.empty()) return std::nullopt;

   auto it = bids_.begin();

   return it->first;
}

std::optional<i64> OrderBook::bestAsk() const {
    if(asks_.empty()) return std::nullopt;

    auto it = asks_.begin();

    return it->first;
}

std::map<i64, PriceLevel, std::greater<i64>>& OrderBook::bids() {
    return bids_;
}

std::map<i64, PriceLevel>& OrderBook::asks() {
    return asks_;
}

const std::map<i64, PriceLevel, std::greater<i64>>& OrderBook::bids() const {
    return bids_;
}

const std::map<i64, PriceLevel>& OrderBook::asks() const {
    return asks_;
}
