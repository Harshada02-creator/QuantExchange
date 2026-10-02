#ifndef QUANTEXCHANGE_ORDERBOOK_H
#define QUANTEXCHANGE_ORDERBOOK_H

#include "Order.h"

#include <deque>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_set>

namespace quantexchange {

// One OrderBook contains one instrument. Price levels are ordered by price and
// each level is FIFO, which implements price-time priority.
class OrderBook {
public:
    explicit OrderBook(std::string symbol);

    const std::string& symbol() const noexcept;

    void addOrder(Order order);
    Order* bestOrder(Side side);
    const Order* bestOrder(Side side) const;
    bool empty(Side side) const;
    bool empty() const noexcept;

    // Cancels an active order in the book by ID. Returns the cancelled order if found, else nullptr.
    std::unique_ptr<Order> cancelOrder(OrderId id);

    // Removes the best order on this side only after it has been fully filled.
    void removeBestFilledOrder(Side side);

private:
    using BuyLevels = std::map<Price, std::deque<Order>, std::greater<Price>>;
    using SellLevels = std::map<Price, std::deque<Order>, std::less<Price>>;

    std::string symbol_;
    BuyLevels buyLevels_;
    SellLevels sellLevels_;
    std::unordered_set<OrderId> activeOrderIds_;
};

}  // namespace quantexchange

#endif  // QUANTEXCHANGE_ORDERBOOK_H
