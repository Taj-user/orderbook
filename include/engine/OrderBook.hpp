#pragma once

#include "Order.hpp"
#include "PriceLevel.hpp"

#include <functional>
#include <map>
#include <unordered_map>

class OrderBook {
        public:
                void insert(Order* order);
                bool cancel(u64 order_id);
                std::optional<Price> best_bid() const;
                std::optional<Price> best_ask() const;
                const std::map<Price, PriceLevel, std::greater<Price>>& bids() const;
                const std::map<Price, PriceLevel>& asks() const;

        private:
                std::map<Price, PriceLevel, std::greater<Price>>         m_bids;                // Bids: highest price first
                std::map<Price, PriceLevel>                              m_asks;                // Asks: lowest price first
                std::unordered_map<u64, Order*>                          m_order_lookup;        // order lookup by id for fast cancellation
};
