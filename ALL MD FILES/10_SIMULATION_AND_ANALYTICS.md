# QuantExchange — Synthetic Market Simulation and Analytics

This is an advanced stage after the core engine, risk, and services are stable.

## Synthetic trader types

### Market maker
Places buy and sell quotes around an estimated fair value.

### Momentum trader
Buys/sells based on simulated short-term movement.

### Random trader
Generates stochastic baseline order flow.

### Institutional trader
Subdivides larger desired trades into smaller orders.

### Risky trader
Attempts oversized orders or repeated limit breaches to exercise risk controls.

## Analytics

Potential measures:
- spread;
- traded volume;
- order imbalance;
- price movement;
- volatility;
- fill ratios;
- execution quality under synthetic regimes;
- risk rejection categories.

## Reproducibility

Simulation runs should record:
- random seed;
- configuration;
- number of traders;
- symbols;
- starting state;
- duration/order count.
