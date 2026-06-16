#include "../../include/feed/MarketDataPublisher.h"

using json = nlohmann::json;

L1Data MarketDataPublisher::getL1(const OrderBook& book, i64 last_trade_price, u64 last_trade_quantity) const {
    L1Data data;
    data.last_trade_price_ = last_trade_price;
    data.last_trade_quantity_ = last_trade_quantity;
    if(!book.bids().empty() && !book.asks().empty()) {
        // best price
        std::optional<i64> best_bid_price = book.bestBid();
        std::optional<i64> best_ask_price = book.bestAsk();
        // best quantity
        auto bid_it = book.bids().begin();
        auto ask_it = book.asks().begin();
        u64 best_bid_quantity = bid_it->second.totalQuantity();
        u64 best_ask_quantity = ask_it->second.totalQuantity();
        // spread
        i64 spread = best_ask_price.value() - best_bid_price.value();

        data.best_bid_price_ = best_bid_price.value();
        data.best_ask_price_ = best_ask_price.value();
        data.best_bid_quantity_ = best_bid_quantity;
        data.best_ask_quantity_ = best_ask_quantity;
        data.spread_ = spread;
    }
    return data;
}

L2Data MarketDataPublisher::getL2(const OrderBook& book, const std::string& symbol, int depth) const {
    L2Data data;
    data.symbol_ = symbol;
    data.depth_ = depth;
    // bids walk
    int count = 0;
    for(auto it = book.bids().begin(); it != book.bids().end() && count < depth; it++, count++) {
        PriceEntry entry;
        entry.price_ = it->first;
        entry.total_quantity_ = it->second.totalQuantity();
        entry.num_orders_ = it->second.size();
        data.bids_.push_back(entry);
    }
    count = 0;
    for(auto it = book.asks().begin(); it != book.asks().end() && count < depth; it++, count++) {
        PriceEntry entry;
        entry.price_ = it->first;
        entry.total_quantity_ = it->second.totalQuantity();
        entry.num_orders_ = it->second.size();
        data.asks_.push_back(entry);
    }
    return data;
}

TradeMessage MarketDataPublisher::getTradeMessage(const Trade& trade) const {
    TradeMessage msg;
    msg.trade_id_ = trade.trade_id_;
    msg.symbol_ = trade.symbol_;
    msg.price_ = trade.price_;
    msg.quantity_ = trade.quantity_;
    msg.timestamp_ = trade.timestamp_;
    return msg;
}

std::string MarketDataPublisher::serializeL1(const L1Data& data) const {
    json j = {
        {"type", "l1"},
        {"best_bid_price", data.best_bid_price_},
        {"best_bid_quantity", data.best_bid_quantity_},
        {"best_ask_price", data.best_ask_price_},
        {"best_ask_quantity", data.best_ask_quantity_},
        {"last_trade_price", data.last_trade_price_},
        {"last_trade_quantity", data.last_trade_quantity_},
        {"spread", data.spread_},
    };
    return j.dump;
}

std::string MarketDataPublisher::serializeL2(const L2Data& data) const {
    json bids_array = json::array();
    for(const auto& entry : data.bids_) {
        bids_array.push_back({
            {"price", entry.price_},
            {"total_quantity", entry.total_quantity_},
            {"num_orders", entry.num_orders_},
        });
    }
    json asks_array = json::array();
    for(const auto& entry : data.asks_) {
        asks_array.push_back({
            {"price", entry.price_},
            {"total_quantity", entry.total_quantity_},
            {"num_orders", entry.num_orders_},
        });
    }

    json j = {
        {"type", "l2"},
        {"symbol", data.symbol_},
        {"bids", bids_array},
        {"asks", asks_array},
        {"depth", data.depth_},
    };
    return j.dump();
}

std::string MarketDataPublisher::serializeTradeMessage(const TradeMessage& msg) const {
    json j = {
        {"type", "trade_message"},
        {"trade_id", msg.trade_id_},
        {"symbol", msg.symbol_},
        {"price", msg.price_},
        {"quantity", msg.quantity_},
        {"timestamp", msg.timestamp_.time_since_epoch().count()},
    };
    return j.dump();
}
