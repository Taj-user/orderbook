#include "../../include/simulation/OrderGenerator.hpp"

#include <chrono>

OrderGenerator::OrderGenerator(MatchingEngine& engine, const OrderGeneratorConfig& config)
        : m_config              (config)
        , m_engine              (engine)
        , m_mid_price           ((m_config.min_price + m_config.max_price) / 2)
        , m_rng                 (m_config.seed)
        , m_price_dist          (m_config.min_price, m_config.max_price)
        , m_quantity_dist       (m_config.min_quantity, m_config.max_quantity)
        , m_side_dist           (0.0, 1.0)
        , m_market_dist         (m_config.market_probability)
{}

void OrderGenerator::start() {
        m_running = true;
        m_thread = std::thread(&OrderGenerator::run, this);
}

void OrderGenerator::stop() {
        m_running = false;
        m_thread.join();
}

void OrderGenerator::run() {
        u64 orders_generated {0};
        while(m_running) {
                Price random_price = m_price_dist(m_rng);
                u32 random_quantity = m_quantity_dist(m_rng);
                double random_side = m_side_dist(m_rng);
                bool market_probability = m_market_dist(m_rng);

                OrderSide side = (random_side < m_config.buy_probability) ? OrderSide::BID : OrderSide::ASK;
                std::optional<Price> price = market_probability ? std::nullopt : std::make_optional<Price>(m_mid_price + random_price);
                OrderType type = market_probability ? OrderType::MARKET : OrderType::LIMIT;

                Order* order = new Order(m_next_order_id++, 0, m_config.symbol, side, type, price, random_quantity);
                m_recent_order_ids.push_back(order->m_order_id);
                m_engine.submit(order);
                orders_generated++;

                if(!m_recent_order_ids.empty()) {
                        std::uniform_int_distribution<size_t> cancel_dist(0, m_recent_order_ids.size() - 1);
                        if(std::bernoulli_distribution(0.1)(m_rng)) {
                                size_t i = cancel_dist(m_rng);
                                m_engine.cancel(m_recent_order_ids[i]);
                                m_recent_order_ids.erase(m_recent_order_ids.begin() + i);
                        }
                }

                if(m_config.num_orders > 0 && orders_generated >= m_config.num_orders) {
                        m_running = false;
                        break;
                }

                std::this_thread::sleep_for(std::chrono::microseconds(1000000) / m_config.orders_per_second);
        }
        m_running = false;
}
//known issue: run() leaks memory. order is allocated on the heap but after its filled, cancelled or rejected, nothing frees the memory
