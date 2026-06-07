#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

typedef int64_t i64;
typedef uint64_t u64;

enum class Side {
    BUY,                                        // Bid
    SELL                                        // Ask
};

enum class OrderType {
    LIMIT,                                      // has a price, rests in book if not immediately matchable
    MARKET                                      // no price, executes immediately against whatever is available
};

enum class OrderStatus {
    NEW,                                        // just created, not yet processed bt the engine
    OPEN,                                       // resting in the book, waiting to be matched
    PARTIALLY_FILLED,                           // some quantity has been executed, remainder is still open
    FILLED,                                     // fully executed, no remaining quantity
    CANCELLED,                                  // removed from the book before full execution
    REJECTED,                                   // never accepted
};

enum class TimeInForce {
    GTC,                                        // Good Till Cancelled - rests in the book indefinately til filled or manually cancelled
    IOC,                                        // Immediate or Cancel - must fill whatever it can right now and any unfilled remainder is cancelled immediately. Never rests in book
    FOK,                                        // Fill or Kill - must be comlpetely filled immediately or the entire order is cancelled
    GTD,                                        // Good Till Date - rests until a specified expiry timestamp
};

using TimePoint = std::chrono::steady_clock::time_point;
using OptionalTimePoint = std::optional<TimePoint>;

struct Order {
    // Identity
    const u64 order_id_;
    const u64 client_order_id_;
    const std::string symbol_;
    // Side & Type
    const Side side_;
    const OrderType order_type_;
    const TimeInForce time_in_force_;
    // Price
    const std::optional<i64> price_;
    // Quantity
    const u64 quantity_original_;
    u64 quantity_remaining_;
    // Timing
    const TimePoint timestamp_;
    const OptionalTimePoint timestamp_expiry_;
    // Status
    OrderStatus status_;
    // Constructor
    Order(u64 order_id, u64 client_order_id, const std::string& symbol, Side side, OrderType order_type, std::optional<i64> price, u64 quantity_original, TimeInForce time_in_force=TimeInForce::GTC, OptionalTimePoint timestamp_expiry=std::nullopt)
    : order_id_ (order_id)
    , client_order_id_ (client_order_id)
    , symbol_ (symbol)
    , side_ (side)
    , order_type_ (order_type)
    , time_in_force_ (time_in_force)
    , price_ (price)
    , quantity_original_ (quantity_original)
    , quantity_remaining_ (quantity_original)
    , timestamp_ (std::chrono::steady_clock::now())
    , timestamp_expiry_ (timestamp_expiry)
    , status_ (OrderStatus::NEW)
    {}
};
