#ifndef QUANTEXCHANGE_TRADE_H
#define QUANTEXCHANGE_TRADE_H

#include "Order.h"

#include <cstdint>
#include <string>

namespace quantexchange {

using TradeId = std::uint64_t;

class Trade {
public:
    Trade(TradeId id,
          std::string symbol,
          Price price,
          Quantity quantity,
          OrderId buyOrderId,
          OrderId sellOrderId,
          std::string buyAccountId,
          std::string sellAccountId);

    TradeId id() const noexcept;
    const std::string& symbol() const noexcept;
    Price price() const noexcept;
    Quantity quantity() const noexcept;
    OrderId buyOrderId() const noexcept;
    OrderId sellOrderId() const noexcept;
    const std::string& buyAccountId() const noexcept;
    const std::string& sellAccountId() const noexcept;

private:
    TradeId id_;
    std::string symbol_;
    Price price_;
    Quantity quantity_;
    OrderId buyOrderId_;
    OrderId sellOrderId_;
    std::string buyAccountId_;
    std::string sellAccountId_;
};

}  // namespace quantexchange

#endif  // QUANTEXCHANGE_TRADE_H
