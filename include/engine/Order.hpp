#pragma once
#include "../common/Types.hpp"
#include <string>
#include <variant>

enum class OrderSide {
        ASK,                                    // Sell
        BID,                                    // Buy
};

enum class OrderType {
        LIMIT,                                  // has a price, rests in book if not immediatly matched
        MARKET,                                 // no price, executes immediately against whatever is availably
};

enum class OrderStatus {
        CANCELLED,                              // removed from book before full execution
        FILLED,                                 // fully executed, no remainding quantity
        NEW,                                    // just created, not yet processed by the engine
        OPEN,                                   // resting in the book waiting to be matched
        PARTIALLY_FILLED,                       // some quantity has been filled, remainder is still open
        REJECTED,                               // never accepted
};

enum class TimeInForce {
        FOK,                                    // Fill Or Kill, must be completely filled immediately or the entire order is cancelled
        GTC,                                    // Good Till Cancelled, rests in the book indefinately till filled or manually cancelled
        GTD,                                    // Good Till Date, rests until a specified expiry timestamp
        IOC,                                    // Immediate Or Cancel, must fill whatever it can immediately and any unfilled remainder is cancelled.
};

struct Order {
        // Order Identity
        const u64 m_order_id;
        const u64 m_client_order_id;
        const std::string m_symbol;

        // Order Side & Type
        const OrderSide m_side;
        const OrderType m_type;
        const TimeInForce m_time_in_force;

        // Price
        const std::optional<Price> m_price;

        // Quantity
        const u64 m_quantity_original;
        u64 m_quantity_remaining;

        // Timing
        const TimePoint m_timestamp;
        const OptionalTimePoint m_timestamp_expiry;

        // Status
        OrderStatus m_status;

        Order(u64 order_id, u64 client_order_id, const std::string& symbol, OrderSide side, OrderType type, std::optional<Price> price, u64 quantity_original
                        , TimeInForce time_in_force=TimeInForce::GTC, OptionalTimePoint timestamp_expiry=std::nullopt)
                : m_order_id (order_id)
                , m_client_order_id (client_order_id)
                , m_symbol (symbol)
                , m_side (side)
                , m_type (type)
                , m_time_in_force (time_in_force)
                , m_price (price)
                , m_quantity_original (quantity_original)
                , m_quantity_remaining (quantity_original)
                , m_timestamp (std::chrono::steady_clock::now())
                , m_timestamp_expiry (timestamp_expiry)
                , m_status (OrderStatus::NEW)
        {}
};

struct CancelRequest {
        u64 order_id;
        u64 timestamp;
};

struct ClientMessage {
        std::variant<Order, CancelRequest> order_state;
};
