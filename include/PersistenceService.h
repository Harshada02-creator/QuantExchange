#ifndef QUANTEXCHANGE_PERSISTENCESERVICE_H
#define QUANTEXCHANGE_PERSISTENCESERVICE_H

#include "IRepository.h"
#include "MatchingEngine.h"
#include "RiskEngine.h"
#include <iostream>

namespace quantexchange {

// Wraps the repository to cleanly decouple in-memory matching 
// from historical database persistence.
class PersistenceService {
public:
    explicit PersistenceService(IRepository* repo) : repo_(repo) {}

    // Persists the results of an order submission
    void persistSubmission(const SubmitResult& res, const RiskEngine& riskEngine) {
        if (!repo_) return;

        try {
            // Persist the final state of the order
            repo_->saveOrder(res.order);

            // If rejected, log the audit record
            if (!res.rejectReason.empty()) {
                repo_->saveAuditRecord(res.order.accountId(), "ORDER_REJECTED", res.rejectReason);
            }

            // Persist any resulting trades and update account states
            for (const auto& trade : res.trades) {
                repo_->saveTrade(trade);
                
                // Save updated buyer state
                repo_->saveAccountState(
                    trade.buyAccountId(), 
                    riskEngine.getAccountState(trade.buyAccountId())
                );
                
                // Save updated seller state
                repo_->saveAccountState(
                    trade.sellAccountId(), 
                    riskEngine.getAccountState(trade.sellAccountId())
                );
            }
        } catch (const std::exception& e) {
            // Handle database errors without corrupting in-memory matching state.
            // The matching and risk updates already happened in-memory.
            // We just log the failure. In a real system, this might go to a dead-letter queue.
            std::cerr << "[Persistence Error] Failed to persist submission for order " 
                      << res.order.id() << ": " << e.what() << "\n";
        }
    }

    void persistCancellation(const CancelResult& res, OrderId orderId, const std::string& accountId) {
        if (!repo_ || !res.success) return;

        try {
            // We don't have the full Order object returned from cancellation, but we could 
            // query it or just log the audit event. A full system might reconstruct a dummy order to save, 
            // but we can simply log the cancellation.
            repo_->saveAuditRecord(accountId, "ORDER_CANCELLED", "Order " + std::to_string(orderId) + " was cancelled.");
        } catch (const std::exception& e) {
            std::cerr << "[Persistence Error] Failed to persist cancellation for order " 
                      << orderId << ": " << e.what() << "\n";
        }
    }

private:
    IRepository* repo_;
};

} // namespace quantexchange

#endif // QUANTEXCHANGE_PERSISTENCESERVICE_H
