# QuantExchange — Later Infrastructure Specification

This file describes the target around the core. It is NOT the immediate task while market-order verification is incomplete.

## Persistence

Recommended relational storage:
- Orders.
- Trades.
- Positions.
- Accounts.
- Risk records.
- Audit records.

## Event stream

Possible event types:
- ORDER_ACCEPTED
- TRADE_EXECUTED
- ORDER_PARTIALLY_FILLED
- ORDER_FILLED
- ORDER_CANCELLED
- ORDER_REJECTED
- MARKET_DATA_UPDATED

Event schemas should be explicit and versionable.

## API

Possible service operations:
- submit order;
- cancel order;
- query order status;
- query trades;
- query order book snapshot;
- stream market data.

API layers must not duplicate core matching rules. They should call the domain engine/services.

## Dashboard

The frontend may visualize:
- order book depth;
- best bid/ask;
- spread;
- recent trades;
- volume;
- trader/account state where appropriate;
- risk status;
- benchmark results.

The dashboard is a presentation layer, not the core achievement.

## Analytics

Potential derived metrics:
- spread;
- volume;
- order imbalance;
- price movement;
- volatility;
- fill ratio;
- execution behavior across synthetic market regimes.
