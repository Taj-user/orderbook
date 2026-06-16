#include "../../include/persistence/RedisClient.h"
#include <unordered_map>

RedisClient::RedisClient(const std::string& uri)
: redis_(uri)
{}

void RedisClient::saveOrder(const Order& order) {
    std::unordered_map<std::string, std::string> map;
    std::string key = "order:" + std::to_string(order.order_id_);
}
