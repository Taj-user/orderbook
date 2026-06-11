#pragma once

#include <vector>
#include "OrderBook.h"
#include "Trade.h"

class MatchingEngine {
    OrderBook book_;
    u64 next_order_id_ = 1;
    u64 next_trade_id_ = 1;
    std::vector<Trade> trade_log_;

    public:
        void submit(Order* order);
        bool cancel(u64 order_id);
        const std::vector<Trade>& trades() const;
        const OrderBook& book() const;
};
