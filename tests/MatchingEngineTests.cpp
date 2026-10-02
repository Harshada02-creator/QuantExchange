#include "MatchingEngine.h"
#include "PersistenceService.h"
#include "PostgresRepository.h"
#include <libpq-fe.h>

#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using namespace quantexchange;

namespace {

class MockRepository : public IRepository {
public:
    std::vector<Order> savedOrders;
    std::vector<Trade> savedTrades;
    std::vector<std::string> auditRecords;
    bool throwOnSave = false;

    void saveOrder(const Order& order) override {
        if (throwOnSave) throw std::runtime_error("DB Error: saveOrder");
        savedOrders.push_back(order);
    }
    void saveTrade(const Trade& trade) override {
        if (throwOnSave) throw std::runtime_error("DB Error: saveTrade");
        savedTrades.push_back(trade);
    }
    void saveAccountState(const std::string&, const AccountState&) override {
        if (throwOnSave) throw std::runtime_error("DB Error: saveAccountState");
    }
    void saveAuditRecord(const std::string&, const std::string& action, const std::string& details) override {
        if (throwOnSave) throw std::runtime_error("DB Error: saveAuditRecord");
        auditRecords.push_back(action + ": " + details);
    }
};

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Exception, typename Callable>
void requireThrows(Callable&& callable, const std::string& message) {
    try {
        std::forward<Callable>(callable)();
    } catch (const Exception&) {
        return;
    } catch (...) {
        throw std::runtime_error(message + " (threw the wrong exception type)");
    }
    throw std::runtime_error(message + " (did not throw)");
}

Order makeOrder(OrderId id,
                Side side,
                Price price,
                Quantity quantity,
                const std::string& symbol = "TCS") {
    return Order(id, symbol, side, price, quantity);
}

Order makeMarketOrder(OrderId id,
                      Side side,
                      Quantity quantity,
                      const std::string& symbol = "TCS") {
    return Order(id, symbol, side, quantity, OrderType::Market);
}

void testOrderLifecycle() {
    Order order = makeOrder(1, Side::Buy, 100, 10);
    require(order.status() == OrderStatus::New, "Order should start New");

    order.validate();
    require(order.status() == OrderStatus::Validated, "Order should be Validated");

    order.accept();
    require(order.status() == OrderStatus::Accepted, "Order should be Accepted");

    order.startMatching();
    require(order.status() == OrderStatus::Matching, "Order should be Matching");

    order.applyFill(4);
    order.completeMatching();
    require(order.status() == OrderStatus::PartiallyFilled,
            "Order should become PartiallyFilled");
    require(order.remainingQuantity() == 6, "Partial fill should leave 6");
    require(order.filledQuantity() == 4, "Filled quantity should be 4");

    order.startMatching();
    order.applyFill(6);
    order.completeMatching();
    require(order.status() == OrderStatus::Filled, "Order should become Filled");
    require(order.remainingQuantity() == 0, "Filled order should have no remainder");
    requireThrows<std::logic_error>([&order] { order.cancel(); },
                                   "Filled order must not be cancellable");
}

void testInvalidOrderTransitions() {
    Order order = makeOrder(1, Side::Buy, 100, 10);

    requireThrows<std::logic_error>([&order] { order.accept(); }, "Cannot accept New order directly");
    requireThrows<std::logic_error>([&order] { order.startMatching(); }, "Cannot start matching New order");
    
    order.validate();
    requireThrows<std::logic_error>([&order] { order.validate(); }, "Cannot validate twice");
    
    order.accept();
    requireThrows<std::logic_error>([&order] { order.applyFill(5); }, "Cannot fill Accepted order");
    
    order.startMatching();
    order.applyFill(5);
    order.completeMatching();
    
    requireThrows<std::logic_error>([&order] { order.applyFill(5); }, "Cannot fill PartiallyFilled order without startMatching");
    
    order.startMatching();
    order.applyFill(5);
}

void testFullMatch() {
    MatchingEngine engine;
    require(engine.submitOrder(makeOrder(1, Side::Sell, 100, 100)).empty(),
            "Uncrossed sell should not create a trade");

    const std::vector<Trade> trades =
        engine.submitOrder(makeOrder(2, Side::Buy, 100, 100));
    require(trades.size() == 1, "Full match should create one trade");
    require(trades[0].quantity() == 100, "Trade quantity should be 100");
    require(trades[0].price() == 100, "Trade should use resting ask price");
    require(trades[0].buyOrderId() == 2, "Trade should identify the buy order");
    require(trades[0].sellOrderId() == 1, "Trade should identify the sell order");

    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr && book->empty(), "Book should be empty after full match");
}

void testPartialFill() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 50));

    const std::vector<Trade> trades =
        engine.submitOrder(makeOrder(2, Side::Buy, 100, 100));
    require(trades.size() == 1, "Partial match should create one trade");
    require(trades[0].quantity() == 50, "Only available quantity should execute");

    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr, "Order book should exist");
    require(book->empty(Side::Sell), "Filled sell should be removed");
    const Order* remainingBuy = book->bestOrder(Side::Buy);
    require(remainingBuy != nullptr, "Buy remainder should rest on the book");
    require(remainingBuy->id() == 2, "The incoming buy should be resting");
    require(remainingBuy->remainingQuantity() == 50,
            "The resting buy should have 50 remaining");
    require(remainingBuy->status() == OrderStatus::PartiallyFilled,
            "Resting remainder should retain its partial-fill status");
}

void testNoMatch() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 30));
    const std::vector<Trade> trades =
        engine.submitOrder(makeOrder(2, Side::Buy, 99, 40));

    require(trades.empty(), "Non-crossing limit orders must not trade");
    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr, "Order book should exist");
    const Order* bestAsk = book->bestOrder(Side::Sell);
    const Order* bestBid = book->bestOrder(Side::Buy);
    require(bestAsk != nullptr, "Ask should remain on the book");
    require(bestBid != nullptr, "Bid should rest on the book");
    require(bestAsk->limitPrice() == 100,
            "Ask should remain at 100");
    require(bestBid->limitPrice() == 99,
            "Bid should rest at 99");
}

void testFifoAtSamePrice() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 10));
    engine.submitOrder(makeOrder(2, Side::Buy, 100, 10));

    const std::vector<Trade> trades =
        engine.submitOrder(makeOrder(3, Side::Sell, 100, 15));
    require(trades.size() == 2, "Sell should match two FIFO buy orders");
    require(trades[0].buyOrderId() == 1 && trades[0].quantity() == 10,
            "Earlier buy at the price level must fill first");
    require(trades[1].buyOrderId() == 2 && trades[1].quantity() == 5,
            "Later buy should receive the remaining fill");

    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr, "Order book should exist");
    const Order* remainingBuy = book->bestOrder(Side::Buy);
    require(remainingBuy != nullptr && remainingBuy->id() == 2,
            "Partially filled second buy should remain at the front");
    require(remainingBuy->remainingQuantity() == 5,
            "Second buy should have 5 remaining");
}

void testHigherBidHasPriority() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 99, 10));
    engine.submitOrder(makeOrder(2, Side::Buy, 101, 10));

    const std::vector<Trade> trades =
        engine.submitOrder(makeOrder(3, Side::Sell, 99, 5));
    require(trades.size() == 1, "Incoming sell should execute against the best bid");
    require(trades[0].buyOrderId() == 2,
            "Higher-priced bid must have priority over the lower bid");
    require(trades[0].price() == 101,
            "Trade should execute at the resting best-bid price");
}

void testWalksMultiplePriceLevels() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 101, 40));
    engine.submitOrder(makeOrder(2, Side::Sell, 102, 30));
    engine.submitOrder(makeOrder(3, Side::Sell, 103, 50));

    const std::vector<Trade> trades =
        engine.submitOrder(makeOrder(4, Side::Buy, 103, 60));
    require(trades.size() == 2, "Incoming order should consume two price levels");
    require(trades[0].price() == 101 && trades[0].quantity() == 40,
            "Best ask should execute first");
    require(trades[1].price() == 102 && trades[1].quantity() == 20,
            "Next ask should execute second");

    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr, "Order book should exist");
    const Order* bestAsk = book->bestOrder(Side::Sell);
    require(bestAsk != nullptr && bestAsk->id() == 2,
            "Unfilled quantity at the second level should remain first");
    require(bestAsk->remainingQuantity() == 10,
            "Second ask should have 10 remaining");
}

void testBooksAreSeparatedBySymbol() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 10, "TCS"));
    const std::vector<Trade> trades =
        engine.submitOrder(makeOrder(2, Side::Buy, 100, 10, "INFY"));

    require(trades.empty(), "Orders for different symbols must not match");
    require(engine.findOrderBook("TCS") != nullptr,
            "TCS should have its own order book");
    require(engine.findOrderBook("INFY") != nullptr,
            "INFY should have its own order book");
}

void testDuplicateOrderIdsAreRejected() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 10));
    requireThrows<std::invalid_argument>(
        [&engine] { engine.submitOrder(makeOrder(1, Side::Buy, 100, 10, "INFY")); },
        "Engine must reject a reused order ID");
}

void testInvalidOrderIsRejected() {
    requireThrows<std::invalid_argument>(
        [] { Order invalid(1, "TCS", Side::Buy, 0, 10); },
        "Order with a zero limit price must be rejected");
}

void testSuccessfulBuyCancellation() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 50));
    CancelResult result = engine.cancelOrder("TCS", 1);
    require(result.success, "Cancellation of active buy order should succeed");
    require(result.remainingQuantity == 50, "Remaining quantity should be 50");
    require(result.filledQuantity == 0, "Filled quantity should be 0");
    require(result.status == OrderStatus::Cancelled, "Status should be Cancelled");
    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr && book->empty(Side::Buy), "Buy book should be empty after cancellation");
}

void testSuccessfulSellCancellation() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 50));
    CancelResult result = engine.cancelOrder("TCS", 1);
    require(result.success, "Cancellation of active sell order should succeed");
    require(result.status == OrderStatus::Cancelled, "Status should be Cancelled");
    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr && book->empty(Side::Sell), "Sell book should be empty after cancellation");
}

void testPartialFillCancellation() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 100));
    engine.submitOrder(makeOrder(2, Side::Sell, 100, 40));
    CancelResult result = engine.cancelOrder("TCS", 1);
    require(result.success, "Cancellation of partially filled order should succeed");
    require(result.remainingQuantity == 60, "Remaining quantity should be 60");
    require(result.filledQuantity == 40, "Filled quantity should be 40");
    require(result.status == OrderStatus::Cancelled, "Status should be Cancelled");
}

void testUnknownOrderCancellation() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 50));
    CancelResult result = engine.cancelOrder("TCS", 2);
    require(!result.success, "Cancellation of unknown order should fail");
    const OrderBook* book = engine.findOrderBook("TCS");
    require(!book->empty(Side::Buy), "Book should not be modified on unknown order cancellation");
}

void testAlreadyFilledOrderCancellation() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 50));
    engine.submitOrder(makeOrder(2, Side::Sell, 100, 50));
    CancelResult result = engine.cancelOrder("TCS", 1);
    require(!result.success, "Already filled order should not be cancellable");
}

void testAlreadyCancelledOrderCancellation() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 50));
    engine.cancelOrder("TCS", 1);
    CancelResult result = engine.cancelOrder("TCS", 1);
    require(!result.success, "Already cancelled order should not be cancellable again");
}

void testCancellationWrongSymbol() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 50, "TCS"));
    CancelResult result = engine.cancelOrder("INFY", 1);
    require(!result.success, "Cancellation with wrong symbol should fail");
}

void testFifoAfterCancellation() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 10));
    engine.submitOrder(makeOrder(2, Side::Buy, 100, 15));
    engine.submitOrder(makeOrder(3, Side::Buy, 100, 20));
    
    engine.cancelOrder("TCS", 2);
    
    std::vector<Trade> trades = engine.submitOrder(makeOrder(4, Side::Sell, 100, 15));
    require(trades.size() == 2, "Should create two trades against the remaining orders in FIFO");
    require(trades[0].buyOrderId() == 1 && trades[0].quantity() == 10, "First trade should be with order 1");
    require(trades[1].buyOrderId() == 3 && trades[1].quantity() == 5, "Second trade should be with order 3");
}

void testRemovalOfEmptyPriceLevel() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 10));
    engine.submitOrder(makeOrder(2, Side::Buy, 99, 10));
    
    engine.cancelOrder("TCS", 1);
    
    const OrderBook* book = engine.findOrderBook("TCS");
    const Order* bestBid = book->bestOrder(Side::Buy);
    require(bestBid != nullptr && bestBid->limitPrice() == 99, "Price level 100 should be removed, leaving 99 as best bid");
}

void testMarketBuyOneLevel() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 50));
    SubmitResult result = engine.submitOrder(makeMarketOrder(2, Side::Buy, 30));
    require(result.trades.size() == 1, "Market buy should create one trade");
    require(result.trades[0].quantity() == 30, "Trade quantity should be 30");
    require(result.trades[0].price() == 100, "Trade price should be 100");
    require(result.order.status() == OrderStatus::Filled, "Market order should be Filled");
}

void testMarketSellOneLevel() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 50));
    SubmitResult result = engine.submitOrder(makeMarketOrder(2, Side::Sell, 30));
    require(result.trades.size() == 1, "Market sell should create one trade");
    require(result.trades[0].quantity() == 30, "Trade quantity should be 30");
    require(result.trades[0].price() == 100, "Trade price should be 100");
    require(result.order.status() == OrderStatus::Filled, "Market order should be Filled");
}

void testMarketBuySweepingMultipleLevels() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 101, 40));
    engine.submitOrder(makeOrder(2, Side::Sell, 102, 30));
    engine.submitOrder(makeOrder(3, Side::Sell, 103, 50));

    SubmitResult result = engine.submitOrder(makeMarketOrder(4, Side::Buy, 100));
    require(result.trades.size() == 3, "Market buy should sweep multiple levels");
    require(result.trades[0].price() == 101 && result.trades[0].quantity() == 40, "First fill 40@101");
    require(result.trades[1].price() == 102 && result.trades[1].quantity() == 30, "Second fill 30@102");
    require(result.trades[2].price() == 103 && result.trades[2].quantity() == 30, "Third fill 30@103");
    require(result.order.status() == OrderStatus::Filled, "Market order should be Filled");
}

void testMarketSellSweepingMultipleLevels() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 103, 40));
    engine.submitOrder(makeOrder(2, Side::Buy, 102, 30));
    engine.submitOrder(makeOrder(3, Side::Buy, 101, 50));

    SubmitResult result = engine.submitOrder(makeMarketOrder(4, Side::Sell, 100));
    require(result.trades.size() == 3, "Market sell should sweep multiple levels");
    require(result.trades[0].price() == 103 && result.trades[0].quantity() == 40, "First fill 40@103");
    require(result.trades[1].price() == 102 && result.trades[1].quantity() == 30, "Second fill 30@102");
    require(result.trades[2].price() == 101 && result.trades[2].quantity() == 30, "Third fill 30@101");
    require(result.order.status() == OrderStatus::Filled, "Market order should be Filled");
}

void testMarketOrderFullyFilled() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 100));
    SubmitResult result = engine.submitOrder(makeMarketOrder(2, Side::Buy, 100));
    require(result.order.status() == OrderStatus::Filled, "Market order fully filled");
    require(result.order.remainingQuantity() == 0, "No remaining quantity");
}

void testMarketOrderPartiallyFilledBecauseLiquidityRunsOut() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 40));
    SubmitResult result = engine.submitOrder(makeMarketOrder(2, Side::Buy, 100));
    require(result.trades.size() == 1, "Should execute against available liquidity");
    require(result.order.status() == OrderStatus::Cancelled, "Unfilled remainder should be Cancelled");
    require(result.order.filledQuantity() == 40, "Filled quantity should be 40");
    require(result.order.remainingQuantity() == 60, "Remaining quantity should be 60");
}

void testMarketOrderWithZeroOpposingLiquidity() {
    MatchingEngine engine;
    SubmitResult result = engine.submitOrder(makeMarketOrder(1, Side::Buy, 100));
    require(result.trades.empty(), "Zero liquidity should create no trades");
    require(result.order.status() == OrderStatus::Cancelled, "Market order should be immediately Cancelled");
}

void testMarketOrderMustNeverRestOnTheBook() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Sell, 100, 40));
    engine.submitOrder(makeMarketOrder(2, Side::Buy, 100));
    
    const OrderBook* book = engine.findOrderBook("TCS");
    require(book->empty(Side::Buy), "Market order remainder must NOT rest on the buy book");
    require(book->empty(Side::Sell), "Sell book should be fully consumed");
}

void testMarketOrderFifoBehavior() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 10));
    engine.submitOrder(makeOrder(2, Side::Buy, 100, 15));

    SubmitResult result = engine.submitOrder(makeMarketOrder(3, Side::Sell, 15));
    require(result.trades.size() == 2, "Market sell should match two FIFO buy orders");
    require(result.trades[0].buyOrderId() == 1 && result.trades[0].quantity() == 10, "Earlier buy fills first");
    require(result.trades[1].buyOrderId() == 2 && result.trades[1].quantity() == 5, "Later buy gets remainder");
}

void testMarketOrderCancellationBehaviorUnaffected() {
    MatchingEngine engine;
    engine.submitOrder(makeOrder(1, Side::Buy, 100, 10));
    engine.cancelOrder("TCS", 1);
    
    SubmitResult result = engine.submitOrder(makeMarketOrder(2, Side::Sell, 10));
    require(result.trades.empty(), "Cancelled order should not provide liquidity");
    require(result.order.status() == OrderStatus::Cancelled, "Market order cancelled due to no liquidity");
}

void testRiskMaxOrderQuantity() {
    MatchingEngine engine;
    RiskConfig config;
    config.maxOrderQuantity = 100;
    engine.riskEngine().setConfig(config);

    SubmitResult res1 = engine.submitOrder(makeOrder(1, Side::Buy, 100, 50));
    require(res1.order.status() == OrderStatus::Accepted, "Order within limit should be accepted");

    SubmitResult res2 = engine.submitOrder(makeOrder(2, Side::Buy, 100, 101));
    require(res2.order.status() == OrderStatus::Rejected, "Order exceeding limit should be rejected");
    require(!res2.rejectReason.empty(), "Should provide a reject reason");
    require(res2.trades.empty(), "Rejected order should generate no trades");
    
    const OrderBook* book = engine.findOrderBook("TCS");
    const Order* bestBuy = book->bestOrder(Side::Buy);
    require(bestBuy->id() == 1, "Rejected order must not rest on the book");
}

void testRiskAvailableCash() {
    MatchingEngine engine;
    RiskConfig config;
    config.checkAvailableCash = true;
    engine.riskEngine().setConfig(config);
    
    engine.riskEngine().setAccountState("default", 1000);
    
    SubmitResult res1 = engine.submitOrder(makeOrder(1, Side::Buy, 50, 10));
    require(res1.order.status() == OrderStatus::Accepted, "Order with sufficient cash should be accepted");
    
    SubmitResult res2 = engine.submitOrder(makeOrder(2, Side::Buy, 200, 10));
    require(res2.order.status() == OrderStatus::Rejected, "Order with insufficient cash should be rejected");
    
    SubmitResult res3 = engine.submitOrder(makeOrder(3, Side::Sell, 200, 10));
    require(res3.order.status() == OrderStatus::Accepted, "Sell order shouldn't be blocked by cash limit in this model");
}

void testRiskPositionLimit() {
    MatchingEngine engine;
    RiskConfig config;
    config.positionLimit = 50;
    engine.riskEngine().setConfig(config);
    
    engine.riskEngine().setAccountPosition("default", "TCS", 20);
    
    SubmitResult res1 = engine.submitOrder(makeOrder(1, Side::Buy, 100, 30));
    require(res1.order.status() == OrderStatus::Accepted, "Order within position limit should be accepted");
    
    SubmitResult res2 = engine.submitOrder(makeOrder(2, Side::Buy, 100, 40));
    require(res2.order.status() == OrderStatus::Rejected, "Order exceeding position limit should be rejected");
}

void testRiskMaxExposure() {
    MatchingEngine engine;
    RiskConfig config;
    config.maxExposure = 5000;
    engine.riskEngine().setConfig(config);
    
    SubmitResult res1 = engine.submitOrder(makeOrder(1, Side::Buy, 100, 40));
    require(res1.order.status() == OrderStatus::Accepted, "Order within exposure should be accepted");
    
    SubmitResult res2 = engine.submitOrder(makeOrder(2, Side::Buy, 100, 60));
    require(res2.order.status() == OrderStatus::Rejected, "Order exceeding exposure should be rejected");
}

void testRiskDailyLossLimit() {
    MatchingEngine engine;
    RiskConfig config;
    config.dailyLossLimit = 100;
    engine.riskEngine().setConfig(config);
    
    engine.riskEngine().setAccountState("TraderA", 1000);
    engine.riskEngine().setAccountState("TraderB", 1000);
    
    engine.submitOrder(Order(1, "TCS", Side::Buy, 100, 1, "TraderA")); 
    engine.submitOrder(Order(2, "TCS", Side::Sell, 100, 1, "TraderB")); 
    
    SubmitResult res = engine.submitOrder(Order(3, "TCS", Side::Buy, 100, 1, "TraderA"));
    require(res.order.status() == OrderStatus::Rejected, "Daily loss limit should block order");
}

void testRiskUpdatesAccountStateAfterTrade() {
    MatchingEngine engine;
    
    engine.riskEngine().setAccountState("TraderA", 1000);
    engine.riskEngine().setAccountState("TraderB", 1000);
    engine.riskEngine().setAccountPosition("TraderB", "TCS", 10);
    
    engine.submitOrder(Order(1, "TCS", Side::Buy, 100, 10, "TraderA"));
    engine.submitOrder(Order(2, "TCS", Side::Sell, 100, 10, "TraderB"));
    
    const auto& accA = engine.riskEngine().getAccountState("TraderA");
    const auto& accB = engine.riskEngine().getAccountState("TraderB");
    
    require(accA.cash == 0, "TraderA spent all cash");
    require(accA.positions.at("TCS") == 10, "TraderA received 10 shares");
    
    require(accB.cash == 2000, "TraderB received cash");
    require(accB.positions.at("TCS") == 0, "TraderB gave up shares (clamped to 0)");
}

void testRiskDemonstration() {
    MatchingEngine engine;
    RiskConfig config;
    config.maxOrderQuantity = 50;
    engine.riskEngine().setConfig(config);

    SubmitResult accepted = engine.submitOrder(makeOrder(1, Side::Buy, 100, 20));
    require(accepted.order.status() == OrderStatus::Accepted, "Accepted order");
    std::cout << "Demonstration - Accepted order status: " << static_cast<int>(accepted.order.status()) << "\n";

    SubmitResult rejected = engine.submitOrder(makeOrder(2, Side::Buy, 100, 100));
    require(rejected.order.status() == OrderStatus::Rejected, "Rejected order");
    std::cout << "Demonstration - Rejected order status: " << static_cast<int>(rejected.order.status()) 
              << ", Reason: " << rejected.rejectReason << "\n";
}

void testPersistenceSuccessfulSave() {
    MatchingEngine engine;
    MockRepository repo;
    PersistenceService persistence(&repo);

    engine.riskEngine().setAccountState("TraderA", 1000);
    engine.riskEngine().setAccountState("TraderB", 1000);
    
    Order buyOrder(1, "TCS", Side::Buy, 100, 10, "TraderA");
    SubmitResult resBuy = engine.submitOrder(buyOrder);
    persistence.persistSubmission(resBuy, engine.riskEngine());

    require(repo.savedOrders.size() == 1, "Order should be saved");
    require(repo.savedTrades.empty(), "No trades yet");

    Order sellOrder(2, "TCS", Side::Sell, 100, 10, "TraderB");
    SubmitResult resSell = engine.submitOrder(sellOrder);
    persistence.persistSubmission(resSell, engine.riskEngine());

    require(repo.savedOrders.size() == 2, "Both orders saved");
    require(repo.savedTrades.size() == 1, "One trade saved");
}

void testPersistenceFailureHandling() {
    MatchingEngine engine;
    MockRepository repo;
    repo.throwOnSave = true; // DB is down
    PersistenceService persistence(&repo);

    engine.riskEngine().setAccountState("TraderA", 1000);
    
    Order buyOrder(1, "TCS", Side::Buy, 100, 10, "TraderA");
    SubmitResult resBuy = engine.submitOrder(buyOrder);
    
    // This should NOT throw an exception, it should catch and log it.
    persistence.persistSubmission(resBuy, engine.riskEngine());
    
    // In-memory state should still be correct!
    const OrderBook* book = engine.findOrderBook("TCS");
    require(book != nullptr, "In-memory state preserved despite DB failure");
    require(book->bestOrder(Side::Buy) != nullptr, "Order exists in memory");
}

void testPostgresRepositoryCompilation() {
    DatabaseConfig config{"localhost", 5432, "qx", "postgres", ""};
    PostgresRepository repo(config);
    repo.setSimulateFailure(false); // Make sure it actually runs
    
    AccountState state;
    state.cash = 1000;
    repo.saveAccountState("TraderA", state);
    repo.saveAccountState("TraderB", state);

    Order order(1, "TCS", Side::Buy, 100, 10, "TraderA");
    repo.saveOrder(order);
    Trade trade(1, "TCS", 100, 10, 1, 2, "TraderA", "TraderB");
    // For Trade, we also need to have order 2 saved (foreign key sell_order_id)
    Order order2(2, "TCS", Side::Sell, 100, 10, "TraderB");
    repo.saveOrder(order2);
    repo.saveTrade(trade);
    
    repo.setSimulateFailure(true);
    bool caught = false;
    try {
        repo.saveOrder(order);
    } catch (...) {
        caught = true;
    }
    require(caught, "PostgresRepository throws when configured to simulate failure");
}

void testRealPostgresIntegration() {
    DatabaseConfig config{"localhost", 5432, "qx", "postgres", ""};
    PostgresRepository repo(config);
    repo.setSimulateFailure(false);
    
    AccountState state;
    state.cash = 10000;
    state.initialCash = 10000;
    state.positions["INFY"] = 20;
    repo.saveAccountState("RealTrader", state);
    repo.saveAccountState("OtherTrader", state);
    
    // Write
    Order order(99, "INFY", Side::Buy, 1200, 50, "RealTrader");
    repo.saveOrder(order);
    
    Order order2(100, "INFY", Side::Sell, 1200, 50, "OtherTrader");
    repo.saveOrder(order2);
    
    Trade trade(999, "INFY", 1200, 20, 99, 100, "RealTrader", "OtherTrader");
    repo.saveTrade(trade);
    
    repo.saveAuditRecord("RealTrader", "TEST_ACTION", "Testing 'quotes' saving");
    
    // Read and verify using libpq directly
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=qx user=postgres");
    require(PQstatus(conn) == CONNECTION_OK, "Test connection OK");
    
    PGresult *r_order = PQexec(conn, "SELECT limit_price, original_quantity FROM orders WHERE order_id = 99");
    require(PQntuples(r_order) == 1, "Order was inserted");
    require(std::string(PQgetvalue(r_order, 0, 0)) == "1200.0000", "Limit price matches");
    require(std::string(PQgetvalue(r_order, 0, 1)) == "50", "Quantity matches");
    PQclear(r_order);
    
    PGresult *r_trade = PQexec(conn, "SELECT price, quantity FROM trades WHERE trade_id = 999");
    require(PQntuples(r_trade) == 1, "Trade was inserted");
    require(std::string(PQgetvalue(r_trade, 0, 0)) == "1200.0000", "Trade price matches");
    require(std::string(PQgetvalue(r_trade, 0, 1)) == "20", "Trade quantity matches");
    PQclear(r_trade);
    
    PGresult *r_acc = PQexec(conn, "SELECT cash FROM accounts WHERE account_id = 'RealTrader'");
    require(PQntuples(r_acc) == 1, "Account was inserted");
    require(std::string(PQgetvalue(r_acc, 0, 0)) == "10000.0000", "Account cash matches");
    PQclear(r_acc);
    
    PGresult *r_pos = PQexec(conn, "SELECT quantity FROM positions WHERE account_id = 'RealTrader' AND symbol = 'INFY'");
    require(PQntuples(r_pos) == 1, "Position was inserted");
    require(std::string(PQgetvalue(r_pos, 0, 0)) == "20", "Position quantity matches");
    PQclear(r_pos);
    
    PGresult *r_aud = PQexec(conn, "SELECT details FROM audit_records WHERE account_id = 'RealTrader' AND action = 'TEST_ACTION'");
    require(PQntuples(r_aud) == 1, "Audit record was inserted");
    require(std::string(PQgetvalue(r_aud, 0, 0)) == "Testing 'quotes' saving", "Audit details matches with quotes");
    PQclear(r_aud);
    
    PQfinish(conn);
}

}  // namespace

int main() {
    const std::vector<std::pair<std::string, std::function<void()>>> tests = {
        {"order lifecycle", testOrderLifecycle},
        {"invalid order transitions", testInvalidOrderTransitions},
        {"full match", testFullMatch},
        {"partial fill", testPartialFill},
        {"no match", testNoMatch},
        {"FIFO priority", testFifoAtSamePrice},
        {"higher bid priority", testHigherBidHasPriority},
        {"multiple price levels", testWalksMultiplePriceLevels},
        {"symbol isolation", testBooksAreSeparatedBySymbol},
        {"duplicate order IDs", testDuplicateOrderIdsAreRejected},
        {"invalid order validation", testInvalidOrderIsRejected},
        {"successful BUY cancellation", testSuccessfulBuyCancellation},
        {"successful SELL cancellation", testSuccessfulSellCancellation},
        {"cancellation of a partially filled order", testPartialFillCancellation},
        {"cancellation of an unknown order", testUnknownOrderCancellation},
        {"cancellation of an already filled order", testAlreadyFilledOrderCancellation},
        {"cancellation of an already cancelled order", testAlreadyCancelledOrderCancellation},
        {"cancellation using the wrong symbol", testCancellationWrongSymbol},
        {"FIFO behavior after cancelling an earlier order", testFifoAfterCancellation},
        {"removal of an empty price level", testRemovalOfEmptyPriceLevel},
        {"Market BUY against one sell level", testMarketBuyOneLevel},
        {"Market SELL against one buy level", testMarketSellOneLevel},
        {"Market BUY sweeping multiple sell price levels", testMarketBuySweepingMultipleLevels},
        {"Market SELL sweeping multiple buy price levels", testMarketSellSweepingMultipleLevels},
        {"Market order fully filled", testMarketOrderFullyFilled},
        {"Market order partially filled because liquidity runs out", testMarketOrderPartiallyFilledBecauseLiquidityRunsOut},
        {"Market order with zero opposing liquidity", testMarketOrderWithZeroOpposingLiquidity},
        {"Market order must never rest on the book", testMarketOrderMustNeverRestOnTheBook},
        {"Existing FIFO behavior remains correct when market orders consume orders", testMarketOrderFifoBehavior},
        {"Existing cancellation behavior remains unchanged", testMarketOrderCancellationBehaviorUnaffected},
        {"Risk check: Max order quantity", testRiskMaxOrderQuantity},
        {"Risk check: Available cash", testRiskAvailableCash},
        {"Risk check: Position limit", testRiskPositionLimit},
        {"Risk check: Max exposure", testRiskMaxExposure},
        {"Risk check: Daily loss limit", testRiskDailyLossLimit},
        {"Risk: Updates account state after successful trade", testRiskUpdatesAccountStateAfterTrade},
        {"Risk: Demonstration of accepted and rejected orders", testRiskDemonstration},
        {"Persistence: Successful save", testPersistenceSuccessfulSave},
        {"Persistence: Failure handling preserves in-memory state", testPersistenceFailureHandling},
        {"Persistence: PostgresRepository builds and runs SQL generation", testPostgresRepositoryCompilation},
        {"Persistence: Real PostgreSQL Integration", testRealPostgresIntegration},
    };

    std::size_t passed = 0;
    for (const auto& test : tests) {
        try {
            test.second();
            ++passed;
            std::cout << "[PASS] " << test.first << '\n';
        } catch (const std::exception& error) {
            std::cerr << "[FAIL] " << test.first << ": " << error.what() << '\n';
            return 1;
        }
    }

    std::cout << passed << " tests passed\n";
    return 0;
}
