#ifndef QUANTEXCHANGE_IREPOSITORY_H
#define QUANTEXCHANGE_IREPOSITORY_H

#include "Order.h"
#include "Trade.h"
#include "RiskEngine.h"

#include <string>

namespace quantexchange {

class IRepository {
public:
    virtual ~IRepository() = default;

    // Called to persist a new or updated order
    virtual void saveOrder(const Order& order) = 0;

    // Called to persist a newly executed trade
    virtual void saveTrade(const Trade& trade) = 0;

    // Called to persist account state updates (cash, positions)
    virtual void saveAccountState(const std::string& accountId, const AccountState& state) = 0;

    // Called to persist an audit/risk record (e.g. order rejection reason)
    virtual void saveAuditRecord(const std::string& accountId, const std::string& action, const std::string& details) = 0;
};

} // namespace quantexchange

#endif // QUANTEXCHANGE_IREPOSITORY_H
