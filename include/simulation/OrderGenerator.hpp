#pragma once

#include "OrderGeneratorConfig.hpp"
#include "../engine/MatchingEngine.hpp"

#include <atomic>
#include <random>
#include <vector>

class OrderGenerator {
        public:
                explicit OrderGenerator(MatchingEngine& engine, const OrderGeneratorConfig& config);
                void start();
                void stop();
                void run();

        private:
                OrderGeneratorConfig                    m_config;
                MatchingEngine&                         m_engine;
                Price                                   m_mid_price {0};
                u64                                     m_next_order_id {1};
                std::vector<u64>                        m_recent_order_ids;
                std::atomic<bool>                       m_running {false};
                std::mt19937                            m_rng;
                std::uniform_int_distribution<Price>    m_price_dist;
                std::uniform_int_distribution<u32>      m_quantity_dist;
                std::uniform_real_distribution<double>  m_side_dist;
                std::bernoulli_distribution             m_market_dist;
                std::thread                             m_thread;
};
