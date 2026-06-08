#pragma once

#include <map>
#include <unordered_map>
#include <functional>
#include "PriceLevel.h"

class OrderBook {
    // Bids: Highest price first
    std::map<i64, PriceLevel, std::greater<i64>> bids_;
    // Asks: Lowest price first
    std::map<i64, PriceLevel> asks_;
    // Order lookup by ID for fast cancellation
    std::unordered_map<u64, Order*> order_map_;

    public:
        void insert(Order* order);                                          // adds to correct side and price leve, creates level if needed
        bool cancel(u64 order_id);                                          // looks up in map, marks as CANCELLED, removes from level
        std::optional<i64> bestBid() const;
        std::optional<i64> bestAsk() const;
        const std::map<i64, PriceLevel, std::greater<i64>>& bids() const;
        const std::map<i64, PriceLevel>& asks() const;
};
