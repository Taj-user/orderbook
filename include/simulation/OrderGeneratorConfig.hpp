#pragma once
#include "../common/Types.hpp"
#include <string>

struct OrderGeneratorConfig {
        u64             num_orders;
        Price           min_price;
        Price           max_price;
        double          buy_probability;
        u32             min_quantity;
        u32             max_quantity;
        u32             orders_per_second;
        std::string     server_ip;
        unsigned int    seed;
        u16             server_port;
};
