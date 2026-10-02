#include "RiskEngine.h"

namespace quantexchange {

bool RiskEngine::checkPreTradeRisk(const Order& order, std::string& rejectReason) {
    if (config_.maxOrderQuantity > 0 && order.originalQuantity() > config_.maxOrderQuantity) {
        rejectReason = "Maximum order quantity exceeded";
        return false;
    }

    auto accIt = accounts_.find(order.accountId());
    if (accIt == accounts_.end()) {
        accounts_[order.accountId()] = AccountState();
        accIt = accounts_.find(order.accountId());
    }

    const auto& acc = accIt->second;

    Price orderValue = 0;
    if (order.type() == OrderType::Limit) {
        orderValue = order.limitPrice() * order.originalQuantity();
    }

    if (config_.checkAvailableCash && order.side() == Side::Buy) {
        if (order.type() == OrderType::Limit && acc.cash < orderValue) {
            rejectReason = "Insufficient cash";
            return false;
        }
    }

    if (config_.positionLimit > 0 && order.side() == Side::Buy) {
        Quantity currentPos = 0;
        auto posIt = acc.positions.find(order.symbol());
        if (posIt != acc.positions.end()) {
            currentPos = posIt->second;
        }
        if (currentPos + order.originalQuantity() > config_.positionLimit) {
            rejectReason = "Position limit exceeded";
            return false;
        }
    }

    if (config_.maxExposure > 0) {
        if (orderValue > config_.maxExposure) {
            rejectReason = "Maximum exposure exceeded";
            return false;
        }
    }

    if (config_.dailyLossLimit > 0) {
        if (acc.initialCash > acc.cash) {
            Price loss = acc.initialCash - acc.cash;
            if (loss >= config_.dailyLossLimit) {
                rejectReason = "Daily loss limit exceeded";
                return false;
            }
        }
    }

    return true;
}

void RiskEngine::applyTrade(const Trade& trade) {
    auto& buyAcc = accounts_[trade.buyAccountId()];
    auto& sellAcc = accounts_[trade.sellAccountId()];
    Price tradeValue = trade.price() * trade.quantity();

    buyAcc.cash -= tradeValue;
    buyAcc.positions[trade.symbol()] += trade.quantity();

    sellAcc.cash += tradeValue;
    if (sellAcc.positions[trade.symbol()] >= trade.quantity()) {
        sellAcc.positions[trade.symbol()] -= trade.quantity();
    } else {
        sellAcc.positions[trade.symbol()] = 0;
    }
}

} // namespace quantexchange
