#ifndef QUANTEXCHANGE_POSTGRESREPOSITORY_H
#define QUANTEXCHANGE_POSTGRESREPOSITORY_H

#include "IRepository.h"
#include <string>

namespace quantexchange {

struct DatabaseConfig {
    std::string host;
    int port;
    std::string dbname;
    std::string user;
    std::string password;
};

// A repository that interfaces with PostgreSQL.
// Note: In this simulation environment without libpq installed, 
// this class simulates execution by throwing exceptions if configured to fail,
// otherwise it conceptually executes the cleanly defined SQL statements.
class PostgresRepository : public IRepository {
public:
    explicit PostgresRepository(const DatabaseConfig& config);
    ~PostgresRepository() override;

    void saveOrder(const Order& order) override;
    void saveTrade(const Trade& trade) override;
    void saveAccountState(const std::string& accountId, const AccountState& state) override;
    void saveAuditRecord(const std::string& accountId, const std::string& action, const std::string& details) override;

    void setSimulateFailure(bool fail) { simulateFailure_ = fail; }

private:
    DatabaseConfig config_;
    bool simulateFailure_ = false;

    // Helper to simulate executing a query. 
    // In a real implementation, this would use libpq/libpqxx.
    void executeQuery(const std::string& sql, const std::string& context);
};

} // namespace quantexchange

#endif // QUANTEXCHANGE_POSTGRESREPOSITORY_H
