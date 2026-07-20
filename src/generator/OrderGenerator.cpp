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
        order.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        order.price = random_price;
        order.quantity = random_quantity;
        if(random_side) order.side |= OrderFlags::BUY;
        else order.side = 0;

        return order;
}

void OrderGenerator::print_lifetime_stats() const {
        if(m_lifetimes.empty()) return;

        constexpr double NS_PER_S = 1'000'000'000.0;
        auto sorted_lifetimes = m_lifetimes;
        std::sort(sorted_lifetimes.begin(), sorted_lifetimes.end());

        // average lifetime
        u64 total = 0;
        for(u64 lifetime : sorted_lifetimes) total += lifetime;
        double average = static_cast<double>(total) / sorted_lifetimes.size();

        // minimum lifetime
        u64 min_lifetime = sorted_lifetimes.front();

        // median lifetime (p50)
        size_t p50_index = sorted_lifetimes.size() * 50 / 100;
        u64 p50 = sorted_lifetimes[p50_index];

        // median + 2 sigma (p90)
        size_t p90_index = sorted_lifetimes.size() * 90 / 100;
        u64 p90 = sorted_lifetimes[p90_index];

        // median + 3 sigma (p99)
        size_t p99_index = sorted_lifetimes.size() * 99 / 100;
        u64 p99 = sorted_lifetimes[p99_index];

        // maximum lifetime
        u64 max_lifetime = sorted_lifetimes.back();

        std::cout << "average: " << average / NS_PER_S << "s\n"
                << "minimum: " << min_lifetime / NS_PER_S << "s\n"
                << "p50: " << p50 / NS_PER_S << "s\n"
                << "p90: " << p90 / NS_PER_S << "s\n"
                << "p99: " << p99 / NS_PER_S << "s\n"
                << "maximum: " << max_lifetime / NS_PER_S << "s\n";
}

void OrderGenerator::receive_results() {
        MatchResult result;
        while(m_tcp.receive_match(result)) {
                u64 now = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

                if(result.bid_complete) m_lifetimes.push_back(now - result.bid_timestamp);
                if(result.ask_complete) m_lifetimes.push_back(now - result.ask_timestamp);

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
        double match_rate = (static_cast<double>(m_matches_received) / sent) * 100;

        std::cout << "Order stats\n";
        std::cout << "sent: " << sent << "\n"                                                                           // number of orders send by generator
                << "matches: " << m_matches_received << "\n"                                                            // matches made by the server
                << "match rate: " << std::fixed << std::setprecision(1) << match_rate << "%\n"                       // match rate: matched orders / orders sent
                << "elapsed: " << us << "/" << std::fixed << std::setprecision(1) << seconds.count() << "s\n"        // time taken to send all orders(in us and s)
                << "throughput: " << orders_per_sec << "orders/sec\n";                                                  // number of orders per second

        std::cout << "\nLifetime stats:\n";
        print_lifetime_stats();
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
