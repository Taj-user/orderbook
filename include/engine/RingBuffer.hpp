#pragma once
#include <cstddef>
#include <condition_variable>
#include <mutex>

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
class RingBuffer {
        public:
                bool push(const T& value);
                bool pop(T& out);
                bool empty() const;
                bool full() const;
                size_t size() const;

        private:
                T data[N];
                size_t head             = 0;
                size_t tail             = 0;
                size_t count            = 0;
                mutable std::mutex      m_mutex;
                std::condition_variable m_condition;
};

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool RingBuffer<T, N>::empty() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return count == 0;
}

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool RingBuffer<T, N>::full() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return count == N;
}

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool RingBuffer<T, N>::push(const T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]() { return count < N; });
        data[tail] = value;
        tail = (tail + 1) & (N - 1);
        count++;
        m_condition.notify_one();
        return true;
}

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool RingBuffer<T, N>::pop(T& out) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]() { return count > 0; });
        out = data[head];
        head = (head + 1) & (N - 1);
        count--;
        m_condition.notify_one();
        return true;
}

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
size_t RingBuffer<T, N>::size() const {
        std::unique_lock<std::mutex> lock(m_mutex);
        return count;
}
