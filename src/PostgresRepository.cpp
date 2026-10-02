#include "PostgresRepository.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <libpq-fe.h>

namespace quantexchange {

PostgresRepository::PostgresRepository(const DatabaseConfig& config) 
    : config_(config) {
}

PostgresRepository::~PostgresRepository() {
}

std::string buildConnStr(const DatabaseConfig& cfg) {
    std::stringstream ss;
    ss << "host=" << cfg.host << " port=" << cfg.port << " dbname=" << cfg.dbname 
       << " user=" << cfg.user;
    if (!cfg.password.empty()) {
        ss << " password=" << cfg.password;
    }
    return ss.str();
}

void PostgresRepository::executeQuery(const std::string& sql, const std::string& context) {
    if (simulateFailure_) {
        throw std::runtime_error("Database connection lost during: " + context);
    }
    
    PGconn *conn = PQconnectdb(buildConnStr(config_).c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::string err = PQerrorMessage(conn);
        PQfinish(conn);
        throw std::runtime_error(context + " connection failed: " + err);
    }
    
    PGresult *res = PQexec(conn, sql.c_str());
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::string err = PQerrorMessage(conn);
        PQclear(res);
        PQfinish(conn);
        throw std::runtime_error(context + " execution failed: " + err);
    }
    
    PQclear(res);
    PQfinish(conn);
}

void PostgresRepository::saveOrder(const Order& order) {
    std::stringstream sql;
    sql << "INSERT INTO orders (order_id, account_id, symbol, side, order_type, limit_price, "
        << "original_quantity, remaining_quantity, status) VALUES ("
        << order.id() << ", '" << order.accountId() << "', '" << order.symbol() << "', '"
        << (order.side() == Side::Buy ? "BUY" : "SELL") << "', '"
        << (order.type() == OrderType::Limit ? "LIMIT" : "MARKET") << "', "
        << order.limitPrice() << ", " << order.originalQuantity() << ", "
        << order.remainingQuantity() << ", '" << static_cast<int>(order.status()) << "') "
        << "ON CONFLICT (order_id) DO UPDATE SET "
        << "remaining_quantity = EXCLUDED.remaining_quantity, "
        << "status = EXCLUDED.status, "
        << "updated_at = CURRENT_TIMESTAMP;";
    
    executeQuery(sql.str(), "saveOrder");
}

void PostgresRepository::saveTrade(const Trade& trade) {
    std::stringstream sql;
    sql << "INSERT INTO trades (trade_id, symbol, price, quantity, buy_order_id, "
        << "sell_order_id, buy_account_id, sell_account_id) VALUES ("
        << trade.id() << ", '" << trade.symbol() << "', " << trade.price() << ", "
        << trade.quantity() << ", " << trade.buyOrderId() << ", " << trade.sellOrderId() << ", '"
        << trade.buyAccountId() << "', '" << trade.sellAccountId() << "');";
        
    executeQuery(sql.str(), "saveTrade");
}

void PostgresRepository::saveAccountState(const std::string& accountId, const AccountState& state) {
    std::stringstream sql;
    sql << "BEGIN; "
        << "INSERT INTO accounts (account_id, cash, initial_cash) VALUES ('"
        << accountId << "', " << state.cash << ", " << state.initialCash << ") "
        << "ON CONFLICT (account_id) DO UPDATE SET cash = EXCLUDED.cash; ";
    
    for (const auto& pos : state.positions) {
        sql << "INSERT INTO positions (account_id, symbol, quantity) VALUES ('"
            << accountId << "', '" << pos.first << "', " << pos.second << ") "
            << "ON CONFLICT (account_id, symbol) DO UPDATE SET "
            << "quantity = EXCLUDED.quantity, updated_at = CURRENT_TIMESTAMP; ";
    }
    sql << "COMMIT;";
    
    // We execute the BEGIN...COMMIT block directly.
    executeQuery(sql.str(), "saveAccountState");
}

void PostgresRepository::saveAuditRecord(const std::string& accountId, const std::string& action, const std::string& details) {
    std::stringstream sql;
    // VERY simple SQL escape for quotes
    std::string safeDetails = details;
    size_t pos = 0;
    while ((pos = safeDetails.find("'", pos)) != std::string::npos) {
        safeDetails.replace(pos, 1, "''");
        pos += 2;
    }

    sql << "INSERT INTO audit_records (account_id, action, details) VALUES ('"
        << accountId << "', '" << action << "', '" << safeDetails << "');";
    
    executeQuery(sql.str(), "saveAuditRecord");
}

} // namespace quantexchange
