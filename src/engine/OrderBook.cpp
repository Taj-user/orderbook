#include "../../include/engine/OrderBook.hpp"
#include "../../include/common/utils.hpp"
#include <algorithm>
#include <cassert>

void OrderBook::add_order(const Order& order) {
        if((order.side & OrderFlags::BUY) != 0) {
                m_bids[to_price_int(order.price)].push(order);
        }
        else {
                m_asks[to_price_int(order.price)].push(order);
        }
}

void OrderBook::match_orders(std::vector<MatchResult>& matches) {
        matches.clear();
        u32 match_qty {};

        while(!m_bids.empty() && !m_asks.empty()) {
                auto best_bid = m_bids.begin();
                auto best_ask = m_asks.begin();

                if(best_bid->first < best_ask->first) break;

                match_qty = std::min(best_bid->second.front().quantity, best_ask->second.front().quantity);
                assert(match_qty > 0);

                best_bid->second.front().quantity -= match_qty;
                best_ask->second.front().quantity -= match_qty;

                MatchResult result;
                result.bid_order_id     = best_bid->second.front().order_id;
                result.ask_order_id     = best_ask->second.front().order_id;
                result.bid_timestamp    = best_bid->second.front().timestamp;
                result.ask_timestamp    = best_ask->second.front().timestamp;
                result.price            = best_ask->first;
                result.match_qty        = match_qty;
                result.bid_complete     = best_bid->second.front().quantity == 0;
                result.ask_complete     = best_ask->second.front().quantity == 0;
                matches.push_back(result);

                if(result.bid_complete) best_bid->second.pop();
                if(result.ask_complete) best_ask->second.pop();

                if(best_bid->second.empty()) best_bid = m_bids.erase(best_bid);
                if(best_ask->second.empty()) best_ask = m_asks.erase(best_ask);
        }
}
