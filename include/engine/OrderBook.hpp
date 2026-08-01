#pragma once
#include "Order.hpp"
#include <map>
#include <queue>
#include <unordered_map>
#include <vector>

class OrderBook {
        public:
                void add_order(const Order& order);
                void match_orders(std::vector<MatchResult>& out);

        private:
                std::map<Price, std::queue<Order>, std::greater<Price>>         m_bids;
                std::map<Price, std::queue<Order>>                              m_asks;
                std::unordered_map<u64, {Price, bool}>                          m_order_index;
};
