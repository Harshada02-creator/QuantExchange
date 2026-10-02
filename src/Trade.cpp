#include "Trade.h"

#include <stdexcept>
#include <utility>

namespace quantexchange {

Trade::Trade(TradeId id,
             std::string symbol,
             Price price,
             Quantity quantity,
             OrderId buyOrderId,
             OrderId sellOrderId,
             std::string buyAccountId,
             std::string sellAccountId)
    : id_(id),
      symbol_(std::move(symbol)),
      price_(price),
      quantity_(quantity),
      buyOrderId_(buyOrderId),
      sellOrderId_(sellOrderId),
      buyAccountId_(std::move(buyAccountId)),
      sellAccountId_(std::move(sellAccountId)) {
    if (id_ == 0) {
        throw std::invalid_argument("Trade ID must be greater than zero");
    }
    if (symbol_.empty()) {
        throw std::invalid_argument("Trade symbol must not be empty");
    }
    if (price_ <= 0) {
        throw std::invalid_argument("Trade price must be greater than zero");
    }
    if (quantity_ == 0) {
        throw std::invalid_argument("Trade quantity must be greater than zero");
    }
    if (buyOrderId_ == 0 || sellOrderId_ == 0) {
        throw std::invalid_argument("Trade order IDs must be greater than zero");
    }
    if (buyOrderId_ == sellOrderId_) {
        throw std::invalid_argument("A trade requires two distinct orders");
    }
}

TradeId Trade::id() const noexcept {
    return id_;
}

const std::string& Trade::symbol() const noexcept {
    return symbol_;
}

Price Trade::price() const noexcept {
    return price_;
}

Quantity Trade::quantity() const noexcept {
    return quantity_;
}

OrderId Trade::buyOrderId() const noexcept {
    return buyOrderId_;
}

OrderId Trade::sellOrderId() const noexcept {
    return sellOrderId_;
}

const std::string& Trade::buyAccountId() const noexcept {
    return buyAccountId_;
}

const std::string& Trade::sellAccountId() const noexcept {
    return sellAccountId_;
}

}  // namespace quantexchange
