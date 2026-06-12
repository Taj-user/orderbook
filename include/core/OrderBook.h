#pragma once

#include <map>
#include <unordered_map>
#include <functional>
#include <optional>
#include "PriceLevel.h"

class OrderBook {
    std::map<i64, PriceLevel, std::greater<i64>> bids_;                     // Bids: Highest price first
    std::map<i64, PriceLevel> asks_;                                        // Asks: Lowest price first
    std::unordered_map<u64, Order*> order_map_;                             // Order lookup by ID for fast cancellation

    public:
        void insert(Order* order);                                          // adds to correct side and price leve, creates level if needed
        bool cancel(u64 order_id);                                          // looks up in map, marks as CANCELLED, removes from level
        std::optional<i64> bestBid() const;                                 // top of bids, nullopt is empty
        std::optional<i64> bestAsk() const;                                 // top of asks, nullopt if empty
        std::map<i64, PriceLevel, std::greater<i64>>& bids();               // exposes bids for MatchingEngine
        std::map<i64, PriceLevel>& asks();                                  // exposes asks for MatchingEngine
        const std::map<i64, PriceLevel, std::greater<i64>>& bids() const;   // exposes bids for MatchingEngine
        const std::map<i64, PriceLevel>& asks() const;                      // exposes asks for MatchingEngine
};
