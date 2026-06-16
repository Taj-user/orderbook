#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include "../core/Order.h"

struct L1Data {
    i64 best_bid_price_ = 0;
    u64 best_bid_quantity_ = 0;
    i64 best_ask_price_ = 0;
    u64 best_ask_quantity_ = 0;
    i64 last_trade_price_ = 0;
    u64 last_trade_quantity_ = 0;
    i64 spread_ = 0;                            // best_ask_price_ - best_bid_price_
};

struct PriceEntry {
    i64 price_;
    u64 total_quantity_;                    // total quantity available at price
    u64 num_orders_;                        // number of individual orders sitting at that price
};

struct L2Data {
    std::string symbol_;
    std::vector<PriceEntry> bids_;
    std::vector<PriceEntry> asks_;
    int depth_ = 0;
};

struct TradeMessage {
    i64 trade_id_;
    std::string symbol_;
    i64 price_;
    u64 quantity_;
    TimePoint timestamp_;
};
