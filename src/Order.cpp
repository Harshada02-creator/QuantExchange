#include "Order.h"

#include <stdexcept>
#include <utility>

namespace quantexchange {

Order::Order(OrderId id,
             std::string symbol,
             Side side,
             Price limitPrice,
             Quantity quantity,
             std::string accountId)
    : id_(id),
      symbol_(std::move(symbol)),
      side_(side),
      limitPrice_(limitPrice),
      originalQuantity_(quantity),
      remainingQuantity_(quantity),
      status_(OrderStatus::New),
      type_(OrderType::Limit),
      accountId_(std::move(accountId)) {
    if (id_ == 0) {
        throw std::invalid_argument("Order ID must be greater than zero");
    }
    if (symbol_.empty()) {
        throw std::invalid_argument("Order symbol must not be empty");
    }
    if (side_ != Side::Buy && side_ != Side::Sell) {
        throw std::invalid_argument("Order side must be Buy or Sell");
    }
    if (limitPrice_ <= 0) {
        throw std::invalid_argument("Limit price must be greater than zero");
    }
    if (originalQuantity_ == 0) {
        throw std::invalid_argument("Order quantity must be greater than zero");
    }
}

Order::Order(OrderId id,
             std::string symbol,
             Side side,
             Quantity quantity,
             OrderType type,
             std::string accountId)
    : id_(id),
      symbol_(std::move(symbol)),
      side_(side),
      limitPrice_(0),
      originalQuantity_(quantity),
      remainingQuantity_(quantity),
      status_(OrderStatus::New),
      type_(type),
      accountId_(std::move(accountId)) {
    if (id_ == 0) {
        throw std::invalid_argument("Order ID must be greater than zero");
    }
    if (symbol_.empty()) {
        throw std::invalid_argument("Order symbol must not be empty");
    }
    if (side_ != Side::Buy && side_ != Side::Sell) {
        throw std::invalid_argument("Order side must be Buy or Sell");
    }
    if (originalQuantity_ == 0) {
        throw std::invalid_argument("Order quantity must be greater than zero");
    }
    if (type_ == OrderType::Limit) {
        throw std::invalid_argument("Limit order must have a limit price");
    }
}

OrderId Order::id() const noexcept {
    return id_;
}

OrderType Order::type() const noexcept {
    return type_;
}

const std::string& Order::symbol() const noexcept {
    return symbol_;
}

Side Order::side() const noexcept {
    return side_;
}

Price Order::limitPrice() const noexcept {
    return limitPrice_;
}

Quantity Order::originalQuantity() const noexcept {
    return originalQuantity_;
}

Quantity Order::remainingQuantity() const noexcept {
    return remainingQuantity_;
}

Quantity Order::filledQuantity() const noexcept {
    return originalQuantity_ - remainingQuantity_;
}

OrderStatus Order::status() const noexcept {
    return status_;
}

const std::string& Order::accountId() const noexcept {
    return accountId_;
}

void Order::validate() {
    if (status_ != OrderStatus::New) {
        throw std::logic_error("Only a new order can be validated");
    }
    status_ = OrderStatus::Validated;
}

void Order::accept() {
    if (status_ != OrderStatus::Validated) {
        throw std::logic_error("Only a validated order can be accepted");
    }
    status_ = OrderStatus::Accepted;
}

void Order::startMatching() {
    if (status_ != OrderStatus::Accepted &&
        status_ != OrderStatus::PartiallyFilled) {
        throw std::logic_error("Only an accepted or partially filled order can start matching");
    }
    status_ = OrderStatus::Matching;
}

void Order::applyFill(Quantity quantity) {
    if (status_ != OrderStatus::Matching) {
        throw std::logic_error("Only an order in matching state can be filled");
    }
    if (quantity == 0) {
        throw std::invalid_argument("Fill quantity must be greater than zero");
    }
    if (quantity > remainingQuantity_) {
        throw std::invalid_argument("Fill quantity exceeds remaining quantity");
    }

    remainingQuantity_ -= quantity;
}

void Order::completeMatching() {
    if (status_ == OrderStatus::Matching) {
        if (remainingQuantity_ == 0) {
            status_ = OrderStatus::Filled;
        } else if (remainingQuantity_ < originalQuantity_) {
            status_ = OrderStatus::PartiallyFilled;
        } else {
            status_ = OrderStatus::Accepted;
        }
    }
}

void Order::cancel() {
    if (status_ != OrderStatus::Accepted &&
        status_ != OrderStatus::PartiallyFilled &&
        status_ != OrderStatus::Matching) {
        throw std::logic_error("Only an active order can be cancelled");
    }
    status_ = OrderStatus::Cancelled;
}

void Order::reject() {
    if (status_ != OrderStatus::New && status_ != OrderStatus::Validated) {
        throw std::logic_error("Only a new or validated order can be rejected");
    }
    status_ = OrderStatus::Rejected;
}

}  // namespace quantexchange
