#include "MatchingEngine.h"

#include <algorithm>
#include <stdexcept>

namespace quantexchange {
namespace {

Side oppositeSide(Side side) {
    if (side == Side::Buy) {
        return Side::Sell;
    }
    if (side == Side::Sell) {
        return Side::Buy;
    }
    throw std::invalid_argument("Side must be Buy or Sell");
}

bool crosses(const Order& incoming, const Order& resting) {
    if (incoming.type() == OrderType::Market) {
        return true;
    }
    if (incoming.side() == Side::Buy) {
        return incoming.limitPrice() >= resting.limitPrice();
    }
    return incoming.limitPrice() <= resting.limitPrice();
}

}  // namespace

MatchingEngine::MatchingEngine() : nextTradeId_(1) {}

SubmitResult MatchingEngine::submitOrder(Order order) {
    if (order.status() != OrderStatus::New) {
        throw std::invalid_argument("Only a new order can be submitted");
    }
    if (seenOrderIds_.find(order.id()) != seenOrderIds_.end()) {
        throw std::invalid_argument("Order ID has already been submitted");
    }

    auto bookEntry = orderBooks_.find(order.symbol());
    if (bookEntry == orderBooks_.end()) {
        bookEntry = orderBooks_
                        .emplace(order.symbol(), OrderBook(order.symbol()))
                        .first;
    }
    const auto idEntry = seenOrderIds_.insert(order.id());
    if (!idEntry.second) {
        throw std::invalid_argument("Order ID has already been submitted");
    }

    order.validate();

    std::string rejectReason;
    if (!riskEngine_.checkPreTradeRisk(order, rejectReason)) {
        order.reject();
        return SubmitResult{{}, std::move(order), rejectReason};
    }

    order.accept();
    order.startMatching();
    OrderBook& book = bookEntry->second;
    std::vector<Trade> trades;

    while (order.remainingQuantity() > 0) {
        Order* restingOrder = book.bestOrder(oppositeSide(order.side()));
        if (restingOrder == nullptr || !crosses(order, *restingOrder)) {
            break;
        }
        if (nextTradeId_ == 0) {
            throw std::overflow_error("Trade ID space has been exhausted");
        }

        const Quantity fillQuantity =
            std::min(order.remainingQuantity(), restingOrder->remainingQuantity());
        const Price executionPrice = restingOrder->limitPrice();
        const OrderId buyOrderId =
            order.side() == Side::Buy ? order.id() : restingOrder->id();
        const OrderId sellOrderId =
            order.side() == Side::Sell ? order.id() : restingOrder->id();

        const std::string& buyAccountId =
            order.side() == Side::Buy ? order.accountId() : restingOrder->accountId();
        const std::string& sellAccountId =
            order.side() == Side::Sell ? order.accountId() : restingOrder->accountId();

        // Construct the trade before mutating either order. If allocation fails,
        // the order book and quantities are still unchanged for this match.
        trades.emplace_back(nextTradeId_,
                            order.symbol(),
                            executionPrice,
                            fillQuantity,
                            buyOrderId,
                            sellOrderId,
                            buyAccountId,
                            sellAccountId);
        ++nextTradeId_;

        restingOrder->startMatching();
        restingOrder->applyFill(fillQuantity);
        order.applyFill(fillQuantity);

        restingOrder->completeMatching();

        if (restingOrder->status() == OrderStatus::Filled) {
            book.removeBestFilledOrder(restingOrder->side());
        }
    }

    order.completeMatching();
    if (order.remainingQuantity() > 0) {
        if (order.type() == OrderType::Market) {
            order.cancel();
        } else {
            book.addOrder(order);
        }
    }
    for (const auto& trade : trades) {
        riskEngine_.applyTrade(trade);
    }

    return SubmitResult{std::move(trades), std::move(order), ""};
}

const OrderBook* MatchingEngine::findOrderBook(
    const std::string& symbol) const {
    const auto entry = orderBooks_.find(symbol);
    return entry == orderBooks_.end() ? nullptr : &entry->second;
}

CancelResult MatchingEngine::cancelOrder(const std::string& symbol, OrderId id) {
    auto bookEntry = orderBooks_.find(symbol);
    if (bookEntry == orderBooks_.end()) {
        return CancelResult{false, 0, 0, OrderStatus::New};
    }

    auto cancelledOrder = bookEntry->second.cancelOrder(id);
    if (!cancelledOrder) {
        return CancelResult{false, 0, 0, OrderStatus::New};
    }

    return CancelResult{
        true,
        cancelledOrder->remainingQuantity(),
        cancelledOrder->filledQuantity(),
        cancelledOrder->status()
    };
}

}  // namespace quantexchange
