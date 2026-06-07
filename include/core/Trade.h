#pragma once

#include <cstdint>
#include <string>
#include <chrono>

typedef uint64_t u64;
typedef int64_t i64;

using TimePoint = std::chrono::steady_clock::time_point;

struct Trade {
    // Identity
    const u64 trade_id_;
    const u64 buy_order_id_;
    const u64 sell_order_id_;

    // Execution details
    const std::string symbol_;
    const i64 price_;
    const u64 quantity_;

    // Timing
    const TimePoint timestamp_;

    // Constructor
    Trade(u64 trade_id, u64 buy_order_id, u64 sell_order_id, const std::string& symbol, i64 price, u64 quantity)
    : trade_id_ (trade_id)
    , buy_order_id_ (buy_order_id)
    , sell_order_id_ (sell_order_id)
    , symbol_ (symbol)
    , price_ (price)
    , quantity_ (quantity)
    , timestamp_ (std::chrono::steady_clock::now())
    {}
};
