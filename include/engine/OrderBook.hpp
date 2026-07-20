#pragma once
#include "Order.hpp"
#include <map>
#include <queue>
#include <vector>

class OrderBook {
        public:
                void add_order(const Order& order);
                std::vector<MatchResult> match_orders();

        private:
                std::map<Price, std::queue<Order>, std::greater<Price>>         m_bids;
                std::map<Price, std::queue<Order>>                              m_asks;
};
