#include "../../include/core/MatchingEngine.h"

void MatchingEngine::submit(Order* order) {
    // Validation
    if(order == nullptr || (order->order_type_ == OrderType::LIMIT && order->price_ == std::nullopt) || (order->order_type_ == OrderType::MARKET && order->price_ != std::nullopt) || order->quantity_original_ <= 0) {
        order->status_ = OrderStatus::REJECTED;
        return;
    }

    // TODO: FOK precheck
}
