#include "../../include/engine/MatchingEngine.hpp"

MatchingEngine::MatchingEngine(OrderBook& book)
        : m_book        (book)
        , m_running     (true)
        , m_worker      (&MatchingEngine::process_orders, this)
{}

MatchingEngine::~MatchingEngine() {
        m_running = false;
        m_worker.join();
}

bool MatchingEngine::pop_result(MatchResult& result) {
                bool status = m_result_queue.pop(result);
                return status;
}

void MatchingEngine::process_orders() {
        while(m_running) {
                Order order;
                if(m_ring_buffer.pop(order)) {
                        m_book.add_order(order);
                        auto match_vec = m_book.match_orders();
                        for(const auto& match : match_vec) {
                                m_result_queue.push(match);
                        }
                }
        }
}

void MatchingEngine::submit_order(const Order& order) {
        m_ring_buffer.push(order);
}
