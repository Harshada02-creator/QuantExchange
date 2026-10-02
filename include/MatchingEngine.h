#ifndef QUANTEXCHANGE_MATCHINGENGINE_H
#define QUANTEXCHANGE_MATCHINGENGINE_H

#include "OrderBook.h"
#include "Trade.h"
#include "RiskEngine.h"

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

namespace quantexchange {

struct CancelResult {
    bool success;
    Quantity remainingQuantity;
    Quantity filledQuantity;
    OrderStatus status;
};

struct SubmitResult {
    std::vector<Trade> trades;
    Order order; // The final state of the submitted order
    std::string rejectReason; // Present if order was rejected by risk engine
    
    operator std::vector<Trade>() const {
        return trades;
    }
    
    bool empty() const {
        return trades.empty();
    }
};

class MatchingEngine {
public:
    MatchingEngine();

    // Submits one new order and returns the trades generated and its final state.
    // Any unfilled quantity of a limit order remains active in the order book.
    SubmitResult submitOrder(Order order);

    // Returns nullptr when no order book has been created for the symbol.
    const OrderBook* findOrderBook(const std::string& symbol) const;

    CancelResult cancelOrder(const std::string& symbol, OrderId id);

    RiskEngine& riskEngine() { return riskEngine_; }
    const RiskEngine& riskEngine() const { return riskEngine_; }

private:
    RiskEngine riskEngine_;
    std::map<std::string, OrderBook> orderBooks_;
    std::unordered_set<OrderId> seenOrderIds_;
    TradeId nextTradeId_;
};

}  // namespace quantexchange

#endif  // QUANTEXCHANGE_MATCHINGENGINE_H
