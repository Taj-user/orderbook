#pragma once

#include "OrderBook.hpp"
#include "SPSCRingBuffer.hpp"
#include "Trade.hpp"
#include <atomic>
#include <thread>
#include <vector>

class MatchingEngine {
        public:
                void submit(Order* order);
                bool cancel(u64 order_id);
                void start();
                void shutdown();
                bool pop_result(Trade& out);
                const OrderBook& book() const;
                const std::vector<Trade>& trades() const;
                u64 next_order_id();

        private:
                void                            process_orders();
                OrderBook                       m_book;
                u64                             m_next_order_id {1};
                u64                             m_next_trade_id {1};
                std::atomic<bool>               m_running {false};
                std::thread                     m_worker;
                std::vector<Trade>              m_trade_log;
                SPSCRingBuffer<Order*, 4096>    m_order_queue;
                SPSCRingBuffer<Trade, 4096>     m_result_queue;
};
