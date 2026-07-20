#include "../../include/network/ClientRegistry.hpp"
#include <iostream>

void ClientRegistry::bind(u64 order_id, SOCKET client) {
        std::lock_guard<std::mutex> lock(m_mutex);
        order_to_client[order_id] = client;
}

void ClientRegistry::unbind(u64 order_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        order_to_client.erase(order_id);
}

SOCKET ClientRegistry::lookup(u64 order_id) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = order_to_client.find(order_id);
        if(it == order_to_client.end()) {
                std::cout << "Lookup failed on order: " << order_id << "\n";
                throw std::runtime_error("Order not found");
        }
        return it->second;
        // return order_to_client.at(order_id);
}
