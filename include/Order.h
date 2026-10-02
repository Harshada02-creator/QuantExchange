#ifndef QUANTEXCHANGE_ORDER_H
#define QUANTEXCHANGE_ORDER_H

#include <cstdint>
#include <string>

namespace quantexchange {

using OrderId = std::uint64_t;
using Quantity = std::uint64_t;
using Price = std::int64_t;

enum class Side {
    Buy,
    Sell
};

enum class OrderStatus {
    New,
    Validated,
    Accepted,
    Matching,
    PartiallyFilled,
    Filled,
    Cancelled,
    Rejected
};

enum class OrderType {
    Limit,
    Market
};

// Price is stored as an integer number of the instrument's smallest price unit
// (for example, paise). This avoids floating-point comparison errors.
class Order {
public:
    Order(OrderId id,
          std::string symbol,
          Side side,
          Price limitPrice,
          Quantity quantity,
          std::string accountId = "default");

    Order(OrderId id,
          std::string symbol,
          Side side,
          Quantity quantity,
          OrderType type,
          std::string accountId = "default");

    OrderId id() const noexcept;
    OrderType type() const noexcept;
    const std::string& symbol() const noexcept;
    Side side() const noexcept;
    Price limitPrice() const noexcept;
    Quantity originalQuantity() const noexcept;
    Quantity remainingQuantity() const noexcept;
    Quantity filledQuantity() const noexcept;
    OrderStatus status() const noexcept;
    const std::string& accountId() const noexcept;

    // Lifecycle transitions. Invalid transitions throw std::logic_error.
    void validate();
    void accept();
    void startMatching();
    void applyFill(Quantity quantity);
    void completeMatching();
    void cancel();
    void reject();

private:
    OrderId id_;
    std::string symbol_;
    Side side_;
    Price limitPrice_;
    Quantity originalQuantity_;
    Quantity remainingQuantity_;
    OrderStatus status_;
    OrderType type_;
    std::string accountId_;
};

}  // namespace quantexchange

#endif  // QUANTEXCHANGE_ORDER_H
