#pragma once
#include "TcpClient.hpp"
#include <vector>

class Client {
        public:
                void run();

        private:
                void create_orders();
                void receive_matches();
                TcpClient m_tcp;
                std::vector<Order> m_orders;
};
