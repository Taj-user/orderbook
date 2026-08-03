#pragma once
#include "../common/Types.hpp"
#include <string>

struct OrderGeneratorConfig {
        u64             num_orders;
        Price           min_price;
        Price           max_price;
        u32             min_quantity;
        u32             max_quantity;
        u32             orders_per_second;
        double          buy_probability;
        double          market_probability;
        unsigned int    seed;
        std::string     symbol;
};
