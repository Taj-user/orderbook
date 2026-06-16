#pragma once
#include <sw/redis++/redis++.h>
#include "../core/Order.h"
#include "../core/Trade.h"
#include <vector>

class RedisClient {
    public:
        RedisClient(const std::string& uri);
        void saveOrder(const Order& order);
        void deleteOrder(u64 order_id);
        void saveTrade(const Trade& trade);
        void publish(const std::string& channel, const std::string& message);
        std::vector<Order*> loadOpenOrder(const std::string& symbols);

    private:
        sw::redis::Redis redis_;
};
