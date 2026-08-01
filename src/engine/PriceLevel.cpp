#include "../../include/engine/PriceLevel.hpp"

PriceLevel::PriceLevel(Price price)
        : m_price (price)
{}

void PriceLevel::add(Order* order) {
        if(order == nullptr) return;
        m_orders.push(order);
}

Order* PriceLevel::front() const {
        if(m_orders.empty()) return nullptr;
        return m_orders.front();
}

void PriceLevel::pop() {
        if(m_orders.empty()) return;
        m_orders.pop();
}

u64 PriceLevel::total_quantity() const {
        u64 total_quantity {};
        std::queue<Order*> temp_orders = m_orders;
        while(!temp_orders.empty()) {
                total_quantity += temp_orders.front()->m_quantity_remaining;
                temp_orders.pop();
        }
        return total_quantity;
}

bool PriceLevel::is_empty() const {
        return m_orders.empty();
}

size_t PriceLevel::size() const {
        return m_orders.size();
}

Price PriceLevel::price() const {
        return m_price;
}
