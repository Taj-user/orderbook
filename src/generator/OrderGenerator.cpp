#include "../../include/generator/OrderGenerator.hpp"
#include <chrono>
#include <iostream>
#include <thread>

OrderGenerator::OrderGenerator(const OrderGeneratorConfig& config)
        : m_config      (config)
        , m_rng         (m_config.seed)
{}

Order OrderGenerator::generate_order() {
        std::uniform_int_distribution<int> price_distrib(m_config.min_price, m_config.max_price);
        Price random_price = price_distrib(m_rng);

        std::uniform_int_distribution<int> quantity_distrib(m_config.min_quantity, m_config.max_quantity);
        u32 random_quantity = quantity_distrib(m_rng);

        std::bernoulli_distribution distrib(m_config.buy_probability);
        bool random_side = distrib(m_rng);

        Order order {};
        order.order_id = m_next_order_id++;
        order.price = random_price;
        order.quantity = random_quantity;
        if(random_side) order.side |= OrderFlags::BUY;
        else order.side = 0;

        return order;
}

void OrderGenerator::receive_results() {
        MatchResult result;
        while(m_tcp.receive_match(result)) {
                ++m_matches_received;
        }
}

void OrderGenerator::run() {
        if(!m_tcp.connect_to_server(m_config.server_ip.c_str(), m_config.server_port)) return;

        std::thread receiver(&OrderGenerator::receive_results, this);
        u64 sent {0};

        auto start = std::chrono::high_resolution_clock::now();

        for(u64 i = 0; i < m_config.num_orders; i++) {
                Order order = generate_order();
                if(!m_tcp.send_order(order)) break;
                ++sent;
                if(m_config.orders_per_second > 0) std::this_thread::sleep_for(std::chrono::duration<double>(1.0 / m_config.orders_per_second));
        }
        m_tcp.disconnect();
        receiver.join();

        auto us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);

        std::chrono::duration<double> seconds = us;
        int orders_per_sec = m_config.num_orders / seconds.count();

        std::cout << "sent: " << sent << " | "                                                                          // number of orders send by generator
                << "matches: " << m_matches_received << " | "                                                           // matches made by the server
                << "elapsed: " << us << "/" << std::fixed << std::setprecision(1) << seconds.count() << "s | "       // time taken to send all orders(in us and s)
                << "throughput: " << orders_per_sec << "orders/sec"                                                     // number of orders per second
                << "\n";                                                                                                // new line character
}

int main(void) {
        WSADATA wsaData {};
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        OrderGeneratorConfig config {
                .num_orders             = 100'000,
                .min_price              = 90'60,
                .max_price              = 105'90,
                .buy_probability        = 0.5,
                .min_quantity           = 5,
                .max_quantity           = 100,
                .orders_per_second      = 0,
                .server_ip              = "127.0.0.1",
                .seed                   = 42,
                .server_port            = 8080
        };

        OrderGenerator generator(config);
        generator.run();

        WSACleanup();

        return 0;
}
