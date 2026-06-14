#include "../../include/feed/MarketDataPublisher.h"

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
