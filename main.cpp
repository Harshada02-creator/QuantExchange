#include "MatchingEngine.h"

#include <iostream>

using namespace quantexchange;

int main() {
    MatchingEngine engine;

    // Prices use the instrument's smallest unit. With paise, 340000 = Rs. 3400.
    engine.submitOrder(Order(1, "TCS", Side::Sell, 340000, 100));
    const std::vector<Trade> trades =
        engine.submitOrder(Order(2, "TCS", Side::Buy, 340000, 60)).trades;

    for (const Trade& trade : trades) {
        std::cout << "Trade " << trade.id() << ": " << trade.symbol() << ' '
                  << trade.quantity() << " shares at " << trade.price()
                  << " paise (buy order " << trade.buyOrderId()
                  << ", sell order " << trade.sellOrderId() << ")\n";
    }

    const OrderBook* book = engine.findOrderBook("TCS");
    if (book != nullptr) {
        const Order* bestAsk = book->bestOrder(Side::Sell);
        if (bestAsk != nullptr) {
            std::cout << "Remaining best ask: " << bestAsk->remainingQuantity()
                      << " shares at " << bestAsk->limitPrice() << " paise\n";
        }
    }

    return 0;
}
