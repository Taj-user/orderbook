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
