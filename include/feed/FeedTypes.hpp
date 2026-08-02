#pragma once

#include "../common/Types.hpp"

#include <string>
#include <vector>

struct L1Data {
        Price   m_best_bid_price        {0};
        u64     m_best_bid_quantity     {0};
        Price   m_best_ask_price        {0};
        u64     m_best_ask_quantity     {0};
        Price   m_last_trade_price      {0};
        u64     m_last_trade_quantity   {0};
        Price   m_spread                {0};
};

struct PriceEntry {
        Price   m_price;
        u64     m_total_quantity;
        u64     m_num_orders;
};

struct L2Data {
        std::string             m_symbol;
        std::vector<PriceEntry> m_bids;
        std::vector<PriceEntry> m_asks;
        int                     m_depth {0};
};

struct TradeMessage {
        Price           m_price         {0};
        u64             m_quantity      {0};
        u64             m_trade_id      {0};
        std::string     m_symbol;
        TimePoint       m_timestamp;
};
