#pragma once
#include "FeedTypes.h"
#include "../core/Trade.h"
#include "../core/OrderBook.h"
#include <nlohmann/json.hpp>                 // uncomment before compiling

class MarketDataPublisher {
    public:
        L1Data getL1(const OrderBook& book, i64 last_trade_price, u64 last_trade_quantity) const;
        L2Data getL2(const OrderBook& book, const std::string& symbol, int depth) const;
        TradeMessage getTradeMessage(const Trade& trade) const;
        std::string serializeL1(const L1Data& data) const;
        std::string serializeL2(const L2Data& data) const;
        std::string serializeTradeMessage(const TradeMessage& msg) const;
};
