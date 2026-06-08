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
        void submit(u64 order_id, u64 client_order_id, const std::string& symbol, Side side, OrderType order_type, std::optional<i64> price, u64 quantity_original, TimeInForce time_in_force=TimeInForce::GTC, OptionalTimePoint timestamp_expiry=std::nullopt);
        bool cancel(u64 order_id);
        const std::vector<Trade>& trades() const;
        const OrderBook& book() const;
};
