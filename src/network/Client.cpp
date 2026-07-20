#include "../../include/network/Client.hpp"
#include "../../include/common/utils.hpp"
#include <iomanip>
#include <iostream>

void Client::run() {
        if(!m_tcp.connect_to_server("127.0.0.1", 8080)) return;

        create_orders();

        for(const auto& order : m_orders) {
                if(!m_tcp.send_order(order)) return;
        }
        receive_matches();
}

void Client::create_orders() {
        m_orders.clear();
        Order o1;
        o1.order_id = 0;
        o1.side |= OrderFlags::BUY;
        o1.price = 50.00;
        o1.quantity = 100;

        Order o2;
        o2.order_id = 1;
        o2.side = 0;
        o2.price = 49.00;
        o2.quantity = 30;

        m_orders.push_back(o1);
        m_orders.push_back(o2);
}

void Client::receive_matches() {
        MatchResult result;
        std::cout << std::fixed << std::setprecision(2);

        while (m_tcp.receive_match(result)) {
                std::cout << "ORDER FILLED: " << result.match_qty << " shares at " << to_price_double(result.price)
                        << " | bid_order_id=" << result.bid_order_id
                        << " | ask_order_id=" << result.ask_order_id << '\n';
        }
}

int main(void) {
        WSADATA wsaData {};
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        Client client;
        client.run();

        WSACleanup();

        return 0;
}
