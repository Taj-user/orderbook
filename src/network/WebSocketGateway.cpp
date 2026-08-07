#include "../../include/network/WebSocketGateway.hpp"

WebSocketGateway::WebSocketGateway(MatchingEngine& engine, int port)
        : m_acceptor(tcp::endpoint(tcp::v4(), port))
        , m_engine(engine)
        , m_port(port)
{}

void WebSocketGateway::start() {
        m_running = true;
        accept();
        m_thread = std::thread([this]() { m_ioc.run(); });
}

void WebSocketGateway::stop() {
        m_ioc.stop();
        m_thread.join();
}

void WebSocketGateway::accept() {
        m_acceptor.async_accept([this](beast::error_code ec, tcp::socket socket) {
                if(!ec) {
                        // create session, do handshake, start reading
                        auto session = std::make_shared<Session>(std::move(socket));
                        session->m_ws.accept(ec);
                        m_sessions.push_back(session);

                        session->m_ws.async_read(session->m_buffer, [this, session](beast::error_code ec, size_t bytes) {
                                if(!ec) {
                                        std::string message = beast::buffers_to_string(session->m_buffer.data());
                                        session->m_buffer.consume(session->m_buffer.size());
                                        on_message(message);
                                }
                        });
                }
                accept();
        });
}
