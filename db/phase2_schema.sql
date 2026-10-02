-- Phase 2: Data Foundation Schema
-- This schema stores metadata and raw historical data for the AI/Quant layer

CREATE TABLE IF NOT EXISTS dataset_metadata (
    version_hash VARCHAR(64) PRIMARY KEY,
    source VARCHAR(255) NOT NULL,
    description TEXT,
    created_at TIMESTAMPTZ DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS historical_ohlcv (
    timestamp TIMESTAMPTZ NOT NULL,
    symbol VARCHAR(20) NOT NULL,
    open NUMERIC NOT NULL,
    high NUMERIC NOT NULL,
    low NUMERIC NOT NULL,
    close NUMERIC NOT NULL,
    volume NUMERIC NOT NULL,
    provenance_hash VARCHAR(64) REFERENCES dataset_metadata(version_hash),
    PRIMARY KEY (symbol, timestamp)
);

CREATE TABLE IF NOT EXISTS historical_trades (
    trade_id BIGSERIAL PRIMARY KEY,
    timestamp TIMESTAMPTZ NOT NULL,
    symbol VARCHAR(20) NOT NULL,
    price NUMERIC NOT NULL,
    quantity NUMERIC NOT NULL,
    is_buyer_maker BOOLEAN,
    provenance_hash VARCHAR(64) REFERENCES dataset_metadata(version_hash)
);
