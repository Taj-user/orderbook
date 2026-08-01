#pragma once

#include "Order.hpp"
#include "PriceLevel.hpp"

#include <functional>
#include <map>
#include <unordered_map>

class OrderBook {
        public:
                void insert(Order* order);                                                      // adds to correct side and price level, created level if needed
                bool cancel(u64 order_id);                                                      // looks up in map, mark as CANCELLED, removes from level
                std::optional<Price> best_bid() const;                                          // top of bids, nullopt if empty
                std::optional<Price> best_ask() const;                                          // top of asks, nullopt if empty
                std::map<Price, PriceLevel, std::greater<Price>>& bids();                       // exposes bids for MatchingEngine
                std::map<Price, PriceLevel>& asks();                                            // exposes asks for MatchingEngine
                const std::map<Price, PriceLevel, std::greater<Price>>& bids() const;           // exposes bids for MatchingEngine
                const std::map<Price, PriceLevel>& asks() const;                                // exposes asks for MatchingEngine

        private:
                std::map<Price, PriceLevel, std::greater<Price>>         m_bids;                // Bids: highest price first
                std::map<Price, PriceLevel>                              m_asks;                // Asks: lowest price first
                std::unordered_map<u64, Order*>                          m_order_lookup;        // order lookup by id for fast cancellation
};
