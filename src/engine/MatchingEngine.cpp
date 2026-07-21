#include "../../include/engine/MatchingEngine.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>

namespace {
        void print_time_ns(u64 ns) {
                std::cout << std::fixed << std::setprecision(2);

                if(ns < 1'000) std::cout << ns << "ns";
                else if(ns < 1'000'000) std::cout << static_cast<double>(ns) / 1'000 << "us";
                else if(ns < 1'000'000'000) std::cout << static_cast<double>(ns) / 1'000'000 << "ms";
                else std::cout << static_cast<double>(ns) / 1'000'000'000 << "s";
        }
}

MatchingEngine::MatchingEngine(OrderBook& book)
        : m_book        (book)
        , m_running     (true)
        , m_worker      (&MatchingEngine::process_orders, this)
{}

MatchingEngine::~MatchingEngine() {
        m_running = false;
        m_result_queue.close();
        m_worker.join();
        print_latency_stats();
}

bool MatchingEngine::pop_result(MatchResult& result) {
                bool status = m_result_queue.pop(result);
                return status;
}

void MatchingEngine::process_orders() {
        std::vector<MatchResult> match_vec;
        match_vec.reserve(32);
        while(m_running) {
                Order order;
                if(!m_ring_buffer.pop(order)) {
                        std::this_thread::yield();
                        continue;
                };

                auto start = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

                m_book.add_order(order);

                m_book.match_orders(match_vec);

                for(const auto& match : match_vec) {
                        m_result_queue.push(match);
                }

                auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() - start;

                m_latencies.push_back(elapsed);
        }
}

void MatchingEngine::print_latency_stats() const {
        if(m_latencies.empty()) return;

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

        // 90th percentile (p90)
        size_t p90_index = sorted_latencies.size() * 90 / 100;
        u64 p90 = sorted_latencies[p90_index];

        // 99th percentile (p99)
        size_t p99_index = sorted_latencies.size() * 99 / 100;
        u64 p99 = sorted_latencies[p99_index];

        // maximum latency
        u64 max_latency = sorted_latencies.back();

        std::cout << "Latency stats\n";

        std::cout << "average: ";
        print_time_ns(static_cast<u64>(average));

        std::cout << "\nminimum: ";
        print_time_ns(min_latency);

        std::cout << "\np50: ";
        print_time_ns(p50);

        std::cout << "\np90: ";
        print_time_ns(p90);

        std::cout << "\np99: ";
        print_time_ns(p99);

        std::cout << "\nmaximum: ";
        print_time_ns(max_latency);
        std::cout << "\n";

}

void MatchingEngine::submit_order(const Order& order) {
        while(!m_ring_buffer.push(order)) std::this_thread::yield();
}

void MatchingEngine::shutdown() {
        m_running = false;
        m_result_queue.close();
}
