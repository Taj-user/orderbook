#pragma once
#include "../common/Types.hpp"
#include <mutex>
#include <unordered_map>
#include <winsock2.h>
#include <ws2tcpip.h>

class ClientRegistry {
        public:
                void bind(u64 order_id, SOCKET client);
                void unbind(u64 order_id);
                SOCKET lookup(u64 order_id) const;

        private:
                std::unordered_map<u64, SOCKET> order_to_client;
                mutable std::mutex m_mutex;
};
