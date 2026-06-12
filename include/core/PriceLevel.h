#pragma once
#include "Order.h"
#include <queue>

class PriceLevel {
    const i64 price_;
    std::queue<Order*> orders_;

    public:
        explicit PriceLevel(i64 price);                         // Constructor
        void add(Order* order);                                 // push to back of queue
        Order* front() const;                                   // returns front without removing
        void pop();                                             // removes front after it's filled
        u64 totalQuantity() const;                              // sum of quantity_remaining_ across all orders
        bool isEmpty() const;                                   // true when queue has no orders
        size_t size() const;                                    // number of orders at this level
        i64 price() const;                                      // returns price
};
