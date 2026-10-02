#ifndef QUANTEXCHANGE_RISKENGINE_H
#define QUANTEXCHANGE_RISKENGINE_H

#include "Order.h"
#include "Trade.h"

#include <string>
#include <unordered_map>

namespace quantexchange {

struct AccountState {
    Price cash = 0;
    Price initialCash = 0;
    std::unordered_map<std::string, Quantity> positions;
};

struct RiskConfig {
    Quantity maxOrderQuantity = 0; // 0 means disabled
    Quantity positionLimit = 0;    // 0 means disabled
    Price maxExposure = 0;         // 0 means disabled
    Price dailyLossLimit = 0;      // 0 means disabled
    bool checkAvailableCash = false;
};

class RiskEngine {
public:
    void setConfig(const RiskConfig& config) { config_ = config; }
    void setAccountState(const std::string& accountId, Price cash) {
        accounts_[accountId].cash = cash;
        accounts_[accountId].initialCash = cash;
    }

    void setAccountPosition(const std::string& accountId, const std::string& symbol, Quantity quantity) {
        accounts_[accountId].positions[symbol] = quantity;
    }

    const AccountState& getAccountState(const std::string& accountId) const {
        return accounts_.at(accountId);
    }

    // Returns true if order passes all risk checks.
    // If false, populates rejectReason.
    bool checkPreTradeRisk(const Order& order, std::string& rejectReason);

    // Updates account states based on an executed trade.
    void applyTrade(const Trade& trade);

private:
    RiskConfig config_;
    std::unordered_map<std::string, AccountState> accounts_;
};

} // namespace quantexchange

#endif // QUANTEXCHANGE_RISKENGINE_H
