#include "../../include/core/OrderBook.h"

void OrderBook::insert(Order* order) {
    if(order->price_) {
        if(order->side_ == Side::BUY) {
            bids_[*order->price_].add(order);
        }
        else {
            asks_[*order->price_].add(order);
        }
        order_map_[order->order_id_] = order;
    }
}
