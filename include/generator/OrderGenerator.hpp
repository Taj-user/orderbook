#pragma once
#include "../engine/Order.hpp"
#include "OrderGeneratorConfig.hpp"
#include "../network/TcpClient.hpp"
#include <atomic>
#include <random>

class OrderGenerator {
        public:
                explicit OrderGenerator(const OrderGeneratorConfig& config);
                void run();

        private:
                Order generate_order();
                void receive_results();
                u64 m_next_order_id {0};
                std::atomic<u64> m_matches_received {0};
                OrderGeneratorConfig m_config;
                TcpClient m_tcp;
                std::mt19937 m_rng;
};
