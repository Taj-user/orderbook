#pragma once
#include "OrderBook.hpp"
#include "RingBuffer.hpp"
#include <atomic>
#include <thread>

class MatchingEngine {
        public:
                MatchingEngine(OrderBook& book);
                ~MatchingEngine();
                bool pop_result(MatchResult& result);
                void submit_order(const Order& order);

        private:
                OrderBook&                      m_book;
                RingBuffer<MatchResult, 4096>     m_result_queue;
                RingBuffer<Order, 4096>           m_ring_buffer;
                std::atomic<bool>               m_running;
                std::thread                     m_worker;
                void process_orders();
};
