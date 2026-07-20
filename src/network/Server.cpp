#include "../../include/network/Server.hpp"
#include "../../include/common/utils.hpp"
#include <iostream>

Server::Server(MatchingEngine& engine, int port)
        : m_engine      (engine)
        , m_running     (true)
{
        WSADATA wsaData;
        if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) throw std::runtime_error("WSAStartup() failed");

        m_socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if(m_socketfd == INVALID_SOCKET) throw std::runtime_error("socket() failed");

        struct sockaddr_in addr;
        addr.sin_family         = AF_INET;
        addr.sin_port           = htons(static_cast<u_short>(port));
        addr.sin_addr.s_addr    = INADDR_ANY;

        int bindfd = bind(m_socketfd, (struct sockaddr*)&addr, sizeof(addr));
        if(bindfd == SOCKET_ERROR) {
                closesocket(m_socketfd);
                throw std::runtime_error("bind() failed");
        }

        int listenfd = listen(m_socketfd, 5);
        if(listenfd == SOCKET_ERROR) {
                closesocket(m_socketfd);
                throw std::runtime_error("listen() failed");
        }
        m_run = std::thread(&Server::run, this);
        m_worker = std::thread(&Server::dispatch_results, this);
}

Server::~Server() {
        m_running = false;
        closesocket(m_socketfd);
        m_run.join();
        m_worker.join();
        for(auto& t : m_threads) {
                if(t.joinable()) t.join();
        }
        WSACleanup();
}

void Server::dispatch_results() {
        while(m_running) {
                MatchResult result;
                m_engine.pop_result(result);
                SOCKET bid_socket = m_registry.lookup(result.bid_order_id);
                SOCKET ask_socket = m_registry.lookup(result.ask_order_id);
                if(bid_socket == ask_socket) send_all(bid_socket, reinterpret_cast<char*>(&result), sizeof(result));
                else {
                        send_all(bid_socket, reinterpret_cast<char*>(&result), sizeof(result));
                        send_all(ask_socket, reinterpret_cast<char*>(&result), sizeof(result));
                }
                if(result.bid_complete) m_registry.unbind(result.bid_order_id);
                if(result.ask_complete) m_registry.unbind(result.ask_order_id);
                std::cout << result.match_qty << " shares at $" << result.price
                        << " | BID=" << result.bid_order_id
                        << " | ASK=" << result.ask_order_id << "\n";
        }
}

void Server::run() {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        while(true) {
                SOCKET clientfd = accept(m_socketfd, (struct sockaddr*)&client_addr, &client_len);
                if(clientfd == INVALID_SOCKET) {
                        closesocket(m_socketfd);
                        return;
                }
                std::thread t([clientfd, this]() -> void {
                        Order order;
                        int recvfd {};
                        while((recvfd = recv(clientfd, reinterpret_cast<char*>(&order), sizeof(order), 0)) > 0) {
                                m_registry.bind(order.order_id, clientfd);
                                m_engine.submit_order(order);
                        }
                        closesocket(clientfd);
                });
                m_threads.push_back(std::move(t));
        }
}

int main(void) {
        OrderBook book;
        MatchingEngine engine(book);
        Server server(engine, 8080);

        std::cin.get();                 // block until Enter is pressed. Or define a signal handler

        return 0;
}
