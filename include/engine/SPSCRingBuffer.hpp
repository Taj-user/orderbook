#pragma once
#include <cstddef>
#include <atomic>

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
class SPSCRingBuffer {
        public:
                bool push(const T& value);
                bool pop(T& out);
                bool empty() const;
                bool full() const;
                constexpr size_t capacity() const;

        private:
                T data[N];
                std::atomic<size_t> m_head {0};
                std::atomic<size_t> m_tail {0};
};

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool SPSCRingBuffer<T, N>::empty() const {
        return m_head.load(std::memory_order_acquire) == m_tail.load(std::memory_order_acquire);
}

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool SPSCRingBuffer<T, N>::full() const {
        size_t tail = m_tail.load(std::memory_order_acquire);
        size_t next_tail = (tail + 1) & (N - 1);
        return next_tail == m_head.load(std::memory_order_acquire);
}

template <typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
constexpr size_t SPSCRingBuffer<T, N>::capacity() const {
        return N - 1;
}

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool SPSCRingBuffer<T, N>::push(const T& value) {
        size_t tail = m_tail.load(std::memory_order_relaxed);
        size_t next_tail = (tail + 1) & (N - 1);
        if(next_tail == m_head.load(std::memory_order_acquire)) return false;

        data[tail] = value;

        m_tail.store(next_tail, std::memory_order_release);

        return true;
}

template<typename T, size_t N>
requires (N > 0 && (N & (N - 1)) == 0)
bool SPSCRingBuffer<T, N>::pop(T& out) {
        size_t head = m_head.load(std::memory_order_relaxed);
        if(head == m_tail.load(std::memory_order_acquire)) return false;

        out = data[head];

        size_t next_head = (head + 1) & (N - 1);
        m_head.store(next_head, std::memory_order_release);

        return true;
}
