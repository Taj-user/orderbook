#pragma once
#include "OrderBook.hpp"
#include "SPSCRingBuffer.hpp"
#include <atomic>
#include <thread>

class MatchingEngine {
        public:
                MatchingEngine(OrderBook& book);
                ~MatchingEngine();
                bool pop_result(MatchResult& result);
                void submit_order(const Order& order);
                void print_latency_stats() const;
                void shutdown();

        private:
                void process_orders();
                OrderBook&                              m_book;
                SPSCRingBuffer<MatchResult, 4096>       m_result_queue;
                SPSCRingBuffer<Order, 4096>             m_ring_buffer;
                std::vector<u64>                        m_latencies;
                std::atomic<bool>                       m_running;
                std::thread                             m_worker;
};
