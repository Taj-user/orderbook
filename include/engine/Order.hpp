#pragma once
#include "../common/Types.hpp"
#include <variant>

namespace OrderFlags {
        constexpr u8 BUY        = 1 << 0;
        constexpr u8 LIMIT      = 1 << 1;
        constexpr u8 IOC        = 1 << 2;
        constexpr u8 FOK        = 1 << 3;
        constexpr u8 HIDDEN     = 1 << 4;
}

enum class MessageType {
        NewOrder,
        Cancel,
};

enum class ResultType {
        Fill,
        CancelAck,
        CancelReject,
};

struct CancelRequest {
        u64 order_id;
        u64 timestamp;
};

struct Order {
        u64     order_id;
        u64     timestamp;
        double  price;
        u32     quantity;
        u8      side = OrderFlags::BUY;
};

struct ClientMessage {
        MessageType type;
        std::variant<Order, CancelRequest> order_state;
};

struct MatchResult {
        u64     bid_order_id;
        u64     ask_order_id;
        u64     bid_timestamp;
        u64     ask_timestamp;
        Price   price;
        u32     match_qty;
        bool    bid_complete;
        bool    ask_complete;
};
