#pragma once

#include "FeedTypes.hpp"
#include "../engine/Trade.hpp"
#include "../engine/OrderBook.hpp"

#include <string>

class MarketDataPublisher {
        public:
                L1Data get_L1(const OrderBook& book, Price last_trade_price, u64 last_trade_quantity) const;
                L2Data get_L2(const OrderBook& book, const std::string& symbol, int depth) const;
                TradeMessage get_trade_message(const Trade& trade) const;
                std::string serialize_L1(const L1Data& data) const;
                std::string serialize_L2(const L2Data& data) const;
                std::string serialize_trade_message(const TradeMessage& message) const;
};
