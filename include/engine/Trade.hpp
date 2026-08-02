#pragma once

#include "../common/Types.hpp"

#include <string>

struct Trade {
        // Trade Identity
        u64 m_trade_id;
        u64 m_bid_order_id;
        u64 m_ask_order_id;

        // Execution details
        std::string m_symbol;
        Price m_price;
        u64 m_quantity;

        // Timing
        TimePoint m_timestamp;

        Trade(u64 trade_id, u64 bid_order_id, u64 ask_order_id, const std::string& symbol, Price price, u64 quantity)
                : m_trade_id            (trade_id)
                , m_bid_order_id        (bid_order_id)
                , m_ask_order_id        (ask_order_id)
                , m_symbol              (symbol)
                , m_price               (price)
                , m_quantity            (quantity)
                , m_timestamp           (std::chrono::steady_clock::now())
        {}
        Trade() = default;
};
