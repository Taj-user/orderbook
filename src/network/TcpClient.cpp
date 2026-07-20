#include "../../include/network/TcpClient.hpp"
#include "../../include/common/utils.hpp"

TcpClient::~TcpClient() {
        disconnect();
}

bool TcpClient::connect_to_server(const char* ip, u16 port) {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(m_socket == INVALID_SOCKET) return false;

        struct sockaddr_in addr {};
        addr.sin_family         = AF_INET;
        addr.sin_port           = htons(port);
        if(inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
                closesocket(m_socket);
                m_socket = INVALID_SOCKET;
                return false;
        }

        if(connect(m_socket, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
                closesocket(m_socket);
                m_socket = INVALID_SOCKET;
                return false;
        }

        return true;
}

bool TcpClient::send_order(const Order& order) {
        return send_all(m_socket, reinterpret_cast<const char*>(&order), sizeof(order));
}

bool TcpClient::receive_match(MatchResult& result) {
        return(recv(m_socket, reinterpret_cast<char*>(&result), sizeof(result), 0)) > 0;
}

void TcpClient::disconnect() {
        if(m_socket != INVALID_SOCKET) closesocket(m_socket);
        m_socket = INVALID_SOCKET;
}
