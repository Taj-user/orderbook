#pragma once

#include "../common/Types.hpp"

#include <string>

struct Trade {
        // Trade Identity
        const u64 m_trade_id;
        const u64 m_bid_order_id;
        const u64 m_ask_order_id;

        // Execution details
        const std::string m_symbol;
        const Price m_price;
        const u64 m_quantity;

        // Timing
        const TimePoint m_timestamp;

        Trade(u64 trade_id, u64 bid_order_id, u64 ask_order_id, const std::string& symbol, Price price, u64 quantity)
                : m_trade_id            (trade_id)
                , m_bid_order_id        (bid_order_id)
                , m_ask_order_id        (ask_order_id)
                , m_symbol              (symbol)
                , m_price               (price)
                , m_quantity            (quantity)
                , m_timestamp           (std::chrono::steady_clock::now())
        {}
};
