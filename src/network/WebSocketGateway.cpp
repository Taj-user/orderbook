#include "../../include/network/WebSocketGateway.hpp"

#include <nlohmann/json.hpp>

#include <algorithm>

using json = nlohmann::json;

WebSocketGateway::WebSocketGateway(MatchingEngine& engine, int port)
        : m_acceptor(m_ioc, tcp::endpoint(tcp::v4(), port))
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

void WebSocketGateway::read(std::shared_ptr<Session> session) {
        session->m_ws.async_read(session->m_buffer, [this, session](beast::error_code ec, size_t) {
                if(!ec) {
                        std::string message = beast::buffers_to_string(session->m_buffer.data());
                        session->m_buffer.consume(session->m_buffer.size());
                        on_message(message);
                        read(session);
                }
                else {
                        m_sessions.erase(std::remove(m_sessions.begin(), m_sessions.end(), session), m_sessions.end());
                }
        });
}

void WebSocketGateway::accept() {
        m_acceptor.async_accept([this](beast::error_code ec, tcp::socket socket) {
                if(!ec) {
                        // create session, do handshake, read from session
                        auto session = std::make_shared<Session>(std::move(socket));
                        session->m_ws.async_accept([this, session](beast::error_code handshake_ec) {
                                if(!handshake_ec) {
                                        m_sessions.push_back(session);
                                        read(session);
                                }
                        });
                }
                accept();
        });
}

void WebSocketGateway::broadcast(const std::string& message) {
        for(auto& session : m_sessions) {
                session->m_ws.write(net::buffer(message));
        }
}

void WebSocketGateway::on_message(const std::string& message) {
        auto j = json::parse(message);
        std::string action = j["action"];
        if(action == "new_order") {
                std::string symbol = j["symbol"].get<std::string>();
                OrderSide side = (j["side"].get<std::string>() == "BID") ? OrderSide::BID : OrderSide::ASK;
                OrderType type = (j["type"].get<std::string>() == "LIMIT") ? OrderType::LIMIT : OrderType::MARKET;
                std::optional<Price> price = std::nullopt;
                if(j.contains("price") && !j["price"].is_null()) {
                        price = j["price"].get<Price>();
                }
                u64 quantity = j["quantity"].get<u64>();
                std::string tif_str = j["tif"].get<std::string>();
                TimeInForce tif = TimeInForce::GTC;
                if(tif_str == "IOC") tif = TimeInForce::IOC;
                else if(tif_str == "FOK") tif = TimeInForce::FOK;
                else if(tif_str == "GTD") tif = TimeInForce::GTD;

                Order* order = new Order(m_engine.next_order_id(), 0, symbol, side, type, price, quantity, tif);
                m_engine.submit(order);

                Trade trade;
                Trade last_trade;
                bool had_trade = false;
                while(m_engine.pop_result(trade)) {
                        had_trade = true;
                        last_trade = trade;
                        auto msg = m_publisher.get_trade_message(trade);
                        broadcast(m_publisher.serialize_trade_message(msg));
                }
                auto l1 = m_publisher.get_L1(m_engine.book(), had_trade ? last_trade.m_price : 0, had_trade ? last_trade.m_quantity : 0);
                broadcast(m_publisher.serialize_L1(l1));
        }
        else if(action == "cancel_order") {
                m_engine.cancel(j["order_id"].get<u64>());
        }
}


