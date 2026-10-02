#include "OrderBook.h"

#include <memory>
#include <stdexcept>
#include <utility>

namespace quantexchange {
namespace {

void validateSide(Side side) {
    if (side != Side::Buy && side != Side::Sell) {
        throw std::invalid_argument("Side must be Buy or Sell");
    }
}

}  // namespace

OrderBook::OrderBook(std::string symbol) : symbol_(std::move(symbol)) {
    if (symbol_.empty()) {
        throw std::invalid_argument("Order book symbol must not be empty");
    }
}

const std::string& OrderBook::symbol() const noexcept {
    return symbol_;
}

void OrderBook::addOrder(Order order) {
    if (order.symbol() != symbol_) {
        throw std::invalid_argument("Order symbol does not match this order book");
    }
    if (order.status() != OrderStatus::Accepted &&
        order.status() != OrderStatus::PartiallyFilled) {
        throw std::invalid_argument("Only active orders can be added to the book");
    }
    if (order.remainingQuantity() == 0) {
        throw std::invalid_argument("A fully filled order cannot rest on the book");
    }
    validateSide(order.side());

    const OrderId id = order.id();
    const auto inserted = activeOrderIds_.insert(id).second;
    if (!inserted) {
        throw std::invalid_argument("Order is already active in this order book");
    }

    try {
        auto addToLevels = [&order](auto& levels) {
            auto level = levels.find(order.limitPrice());
            if (level == levels.end()) {
                level = levels.emplace(order.limitPrice(), std::deque<Order>()).first;
            }
            try {
                level->second.push_back(std::move(order));
            } catch (...) {
                if (level->second.empty()) {
                    levels.erase(level);
                }
                throw;
            }
        };

        if (order.side() == Side::Buy) {
            addToLevels(buyLevels_);
        } else {
            addToLevels(sellLevels_);
        }
    } catch (...) {
        activeOrderIds_.erase(id);
        throw;
    }
}

Order* OrderBook::bestOrder(Side side) {
    validateSide(side);
    if (side == Side::Buy) {
        if (buyLevels_.empty()) {
            return nullptr;
        }
        return &buyLevels_.begin()->second.front();
    }

    if (sellLevels_.empty()) {
        return nullptr;
    }
    return &sellLevels_.begin()->second.front();
}

const Order* OrderBook::bestOrder(Side side) const {
    validateSide(side);
    if (side == Side::Buy) {
        if (buyLevels_.empty()) {
            return nullptr;
        }
        return &buyLevels_.begin()->second.front();
    }

    if (sellLevels_.empty()) {
        return nullptr;
    }
    return &sellLevels_.begin()->second.front();
}

bool OrderBook::empty(Side side) const {
    validateSide(side);
    return side == Side::Buy ? buyLevels_.empty() : sellLevels_.empty();
}

bool OrderBook::empty() const noexcept {
    return buyLevels_.empty() && sellLevels_.empty();
}

std::unique_ptr<Order> OrderBook::cancelOrder(OrderId id) {
    if (activeOrderIds_.find(id) == activeOrderIds_.end()) {
        return nullptr;
    }

    auto findAndRemove = [&](auto& levels) -> std::unique_ptr<Order> {
        for (auto levelIt = levels.begin(); levelIt != levels.end(); ++levelIt) {
            auto& queue = levelIt->second;
            for (auto qIt = queue.begin(); qIt != queue.end(); ++qIt) {
                if (qIt->id() == id) {
                    qIt->cancel();
                    auto cancelledOrder = std::make_unique<Order>(*qIt);
                    queue.erase(qIt);
                    if (queue.empty()) {
                        levels.erase(levelIt);
                    }
                    activeOrderIds_.erase(id);
                    return cancelledOrder;
                }
            }
        }
        return nullptr;
    };

    if (auto order = findAndRemove(buyLevels_)) {
        return order;
    }
    return findAndRemove(sellLevels_);
}

void OrderBook::removeBestFilledOrder(Side side) {
    validateSide(side);
    Order* order = bestOrder(side);
    if (order == nullptr) {
        throw std::out_of_range("Cannot remove an order from an empty side");
    }
    if (order->status() != OrderStatus::Filled) {
        throw std::logic_error("Only a fully filled best order can be removed");
    }

    activeOrderIds_.erase(order->id());

    if (side == Side::Buy) {
        auto level = buyLevels_.begin();
        level->second.pop_front();
        if (level->second.empty()) {
            buyLevels_.erase(level);
        }
    } else {
        auto level = sellLevels_.begin();
        level->second.pop_front();
        if (level->second.empty()) {
            sellLevels_.erase(level);
        }
    }
}

}  // namespace quantexchange
