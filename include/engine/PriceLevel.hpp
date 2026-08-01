#pragma once

#include "Order.hpp"

#include <queue>

class PriceLevel {
        public:
                explicit PriceLevel(Price price);
                void add(Order* order);
                Order* front() const;
                void pop();
                u64 total_quantity() const;
                bool is_empty() const;
                size_t size() const;
                Price price() const;

        private:
                const Price m_price;
                std::queue<Order*> m_orders;
};
