#pragma once
#include "ClientRegistry.hpp"
#include "../engine/MatchingEngine.hpp"

class Server {
        public:
                Server(MatchingEngine& engine, int port);
                ~Server();
                void dispatch_results();
                void run();

        private:
                ClientRegistry                  m_registry;
                MatchingEngine&                 m_engine;
                SOCKET                          m_socketfd;
                std::atomic<bool>               m_running;
                std::thread                     m_run;
                std::thread                     m_worker;
                std::vector<std::thread>        m_threads;
};
