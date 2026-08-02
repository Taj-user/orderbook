#include "../../include/feed/MarketDataPublisher.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

L1Data MarketDataPublisher::get_L1(const OrderBook& book, Price last_trade_price, u64 last_trade_quantity) const {
        L1Data data;
        data.m_last_trade_price = last_trade_price;
        data.m_last_trade_quantity = last_trade_quantity;

        if(!book.bids().empty() && !book.asks().empty()) {
                std::optional<Price> best_bid_price = book.best_bid();
                std::optional<Price> best_ask_price = book.best_ask();

                auto bid_it = book.bids().begin();
                auto ask_it = book.asks().begin();
                u64 best_bid_quantity = bid_it->second.total_quantity();
                u64 best_ask_quantity = ask_it->second.total_quantity();

                Price spread = best_ask_price.value() - best_bid_price.value();

                data.m_best_bid_price = best_bid_price.value();
                data.m_best_bid_quantity = best_bid_quantity;
                data.m_best_ask_price = best_ask_price.value();
                data.m_best_ask_quantity = best_ask_quantity;
                data.m_spread = spread;
        }
        return data;
}

L2Data MarketDataPublisher::get_L2(const OrderBook& book, const std::string& symbol, int depth) const {
        L2Data data;
        data.m_symbol = symbol;
        data.m_depth = depth;

        int count = 0;
        for(auto it = book.bids().begin(); it != book.bids().end() && count < depth; it++, count++) {
                PriceEntry entry;
                entry.m_price = it->first;
                entry.m_total_quantity = it->second.total_quantity();
                entry.m_num_orders = it->second.size();
                data.m_bids.push_back(entry);
        }

        count = 0;
        for(auto it = book.asks().begin(); it != book.asks().end() && count < depth; it++, count++) {
                PriceEntry entry;
                entry.m_price = it->first;
                entry.m_total_quantity = it->second.total_quantity();
                entry.m_num_orders = it->second.size();
                data.m_asks.push_back(entry);
        }
        return data;
}

TradeMessage MarketDataPublisher::get_trade_message(const Trade& trade) const {
        TradeMessage message;
        message.m_trade_id = trade.m_trade_id;
        message.m_symbol = trade.m_symbol;
        message.m_price = trade.m_price;
        message.m_quantity = trade.m_quantity;
        message.m_timestamp = trade.m_timestamp;
        return message;
}

std::string MarketDataPublisher::serialize_L1(const L1Data& data) const {
        json j = {
                {"type", "l1"},
                {"best_bid_price", data.m_best_bid_price},
                {"best_bid_quantity", data.m_best_bid_quantity},
                {"best_ask_price", data.m_best_ask_price},
                {"best_ask_quantity", data.m_best_ask_quantity},
                {"last_trade_price", data.m_last_trade_price},
                {"last_trade_quantity", data.m_last_trade_quantity},
                {"spread", data.m_spread},
        };
        return j.dump();
}

std::string MarketDataPublisher::serialize_L2(const L2Data& data) const {
        json bids_array = json::array();
        for(const auto& entry : data.m_bids) {
                bids_array.push_back({
                        {"price", entry.m_price},
                        {"total_quantity", entry.m_total_quantity},
                        {"num_orders", entry.m_num_orders},
                });
        }

        json asks_array = json::array();
        for(const auto& entry : data.m_asks) {
                asks_array.push_back({
                        {"price", entry.m_price},
                        {"total_quantity", entry.m_total_quantity},
                        {"num_orders", entry.m_num_orders},
                });
        }

        json j = {
                {"type", "l2"},
                {"symbol", data.m_symbol},
                {"bids", bids_array},
                {"asks", asks_array},
                {"depth", data.m_depth},
        };
        return j.dump();
}

std::string MarketDataPublisher::serialize_trade_message(const TradeMessage& message) const {
    json j = {
        {"type", "trade_message"},
        {"trade_id", message.m_trade_id},
        {"symbol", message.m_symbol},
        {"price", message.m_price},
        {"quantity", message.m_quantity},
        {"timestamp", message.m_timestamp.time_since_epoch().count()},
    };
    return j.dump();
}
