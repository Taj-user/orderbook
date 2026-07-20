#include "../../include/common/utils.hpp"
#include <cmath>

bool send_all(SOCKET fd, const char* buffer, int length) {
        int total = 0;
        while(total < length) {
                int sent = send(fd, buffer + total, length - total , 0);
                if(sent == -1) return false;
                total += sent;
        }
        return true;
}

Price to_price_int(double d) {
        return static_cast<Price>(std::round(d * PRICE_SCALE));
}

double to_price_double(Price p) {
        return static_cast<double>(p) / PRICE_SCALE;
}
