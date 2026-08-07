#pragma once

#include "../engine/MatchingEngine.hpp"
#include "../feed/MarketDataPublisher.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>

#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class WebSocketGateway {
        public:
                WebSocketGateway(MatchingEngine& engine, int port);
                void start();
                void stop();
                void broadcast(const std::string& message);

        private:
                struct Session {
                        websocket::stream<beast::tcp_stream>    m_ws;
                        beast::flat_buffer                      m_buffer;

                        explicit Session(tcp::socket socket)
                                : m_ws(std::move(socket))
                        {}
                };

                void read(std::shared_ptr<Session> session);
                void accept();
                void on_message(const std::string& message);
                net::io_context                         m_ioc;                                  // asio event loop
                tcp::acceptor                           m_acceptor;                             // listens for incoming connections
                MatchingEngine&                         m_engine;                               // to submit orders and drain results
                MarketDataPublisher                     m_publisher;                            // to format outbound messages
                std::vector<std::shared_ptr<Session>>   m_sessions;                             // connected clients
                std::thread                             m_thread;                               // runs the io_context
                std::atomic<bool>                       m_running {false};
                int                                     m_port {};
};
