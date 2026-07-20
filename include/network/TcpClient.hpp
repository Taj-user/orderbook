#pragma once
#include "../engine/Order.hpp"
#include "../common/Types.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>

class TcpClient {
        public:
                TcpClient() = default;
                ~TcpClient();
                bool connect_to_server(const char* ip, u16 port);
                bool send_order(const Order& order);
                bool receive_match(MatchResult& result);
                void disconnect();

        private:
                SOCKET m_socket = INVALID_SOCKET;
};
