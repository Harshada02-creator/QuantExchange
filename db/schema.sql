CREATE TABLE accounts (
    account_id VARCHAR(50) PRIMARY KEY,
    cash NUMERIC(18, 4) NOT NULL DEFAULT 0,
    initial_cash NUMERIC(18, 4) NOT NULL DEFAULT 0,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE positions (
    account_id VARCHAR(50) REFERENCES accounts(account_id),
    symbol VARCHAR(20) NOT NULL,
    quantity BIGINT NOT NULL DEFAULT 0,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (account_id, symbol)
);

CREATE TABLE orders (
    order_id BIGINT PRIMARY KEY,
    account_id VARCHAR(50) REFERENCES accounts(account_id),
    symbol VARCHAR(20) NOT NULL,
    side VARCHAR(4) NOT NULL, -- 'BUY' or 'SELL'
    order_type VARCHAR(10) NOT NULL, -- 'LIMIT' or 'MARKET'
    limit_price NUMERIC(18, 4) NOT NULL,
    original_quantity BIGINT NOT NULL,
    remaining_quantity BIGINT NOT NULL,
    status VARCHAR(20) NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE trades (
    trade_id BIGINT PRIMARY KEY,
    symbol VARCHAR(20) NOT NULL,
    price NUMERIC(18, 4) NOT NULL,
    quantity BIGINT NOT NULL,
    buy_order_id BIGINT REFERENCES orders(order_id),
    sell_order_id BIGINT REFERENCES orders(order_id),
    buy_account_id VARCHAR(50) REFERENCES accounts(account_id),
    sell_account_id VARCHAR(50) REFERENCES accounts(account_id),
    executed_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE audit_records (
    id SERIAL PRIMARY KEY,
    account_id VARCHAR(50) REFERENCES accounts(account_id),
    action VARCHAR(50) NOT NULL,
    details TEXT,
    recorded_at TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP
);
