#include "../../include/core/PriceLevel.h"

PriceLevel::PriceLevel(i64 price)
: price_ (price)
{}

void PriceLevel::add(Order* order) {
    if(order == nullptr) return;
    orders_.push(order);
}

Order* PriceLevel::front() const {
    if(orders_.empty()) return nullptr;
    return orders_.front();
}

void PriceLevel::pop() {
    orders_.pop();
}

u64 PriceLevel::totalQuantity() const {
    u64 total_quantity = 0;
    std::queue<Order*> temp = orders_;
    while(!temp.empty()) {
        total_quantity += temp.front()->quantity_remaining_;
        temp.pop();
    }

    return total_quantity;
}

bool PriceLevel::isEmpty() const {
    return orders_.empty();
}

size_t PriceLevel::size() const {
    return orders_.size();
}

i64 PriceLevel::price() const {
    return price_;
}
