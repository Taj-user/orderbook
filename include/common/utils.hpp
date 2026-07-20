#pragma once
#include "Types.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>

bool send_all(SOCKET fd, const char* buffer, int length);
Price to_price_int(double d);
double to_price_double(Price p);
