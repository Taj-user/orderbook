#include "../../include/engine/MatchingEngine.hpp"
#include <algorithm>
#include <iostream>

MatchingEngine::MatchingEngine(OrderBook& book)
        : m_book        (book)
        , m_running     (true)
        , m_worker      (&MatchingEngine::process_orders, this)
{}

MatchingEngine::~MatchingEngine() {
        m_running = false;
        m_ring_buffer.close();
        m_result_queue.close();
        m_worker.join();
        print_latency_stats();
}

bool MatchingEngine::pop_result(MatchResult& result) {
                bool status = m_result_queue.pop(result);
                return status;
}

void MatchingEngine::process_orders() {
        while(true) {
                Order order;
                if(!m_ring_buffer.pop(order)) break;

                auto start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

                m_book.add_order(order);

                auto match_vec = m_book.match_orders();

                for(const auto& match : match_vec) {
                        m_result_queue.push(match);
                }

                auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - start;

                m_latencies.push_back(elapsed);
        }
}

void MatchingEngine::print_latency_stats() const {                      // Consider moving latency logic out of MatchingEngine later
        if(m_latencies.empty()) return;

        constexpr double NS_PER_S = 1'000'000.0;
        auto sorted_latencies = m_latencies;
        std::sort(sorted_latencies.begin(), sorted_latencies.end());

        // average latency
        u64 total = 0;
        for(u64 latency : sorted_latencies) total += latency;
        double average = static_cast<double>(total) / sorted_latencies.size();

        // minimum latency
        u64 min_latency = sorted_latencies.front();

        // median latency (p50)
        size_t p50_index = sorted_latencies.size() * 50 / 100;
        u64 p50 = sorted_latencies[p50_index];

        // median + 2 sigma (p90)
        size_t p90_index = sorted_latencies.size() * 90 / 100;
        u64 p90 = sorted_latencies[p90_index];

        // median + 3 sigma (p99)
        size_t p99_index = sorted_latencies.size() * 99 / 100;
        u64 p99 = sorted_latencies[p99_index];

        // maximum latency
        u64 max_latency = sorted_latencies.back();

        std::cout << "Latency Stats\n"
                << "average: " << average / NS_PER_S << "s\n"
                << "minimum: " << min_latency / NS_PER_S << "s\n"
                << "p50: " << p50 / NS_PER_S << "s\n"
                << "p90: " << p90 / NS_PER_S << "s\n"
                << "p99: " << p99 / NS_PER_S << "s\n"
                << "maximum: " << max_latency / NS_PER_S << "s\n";
}

void MatchingEngine::submit_order(const Order& order) {
        m_ring_buffer.push(order);
}

void MatchingEngine::shutdown() {
        m_running = false;
        m_ring_buffer.close();
        m_result_queue.close();
}
