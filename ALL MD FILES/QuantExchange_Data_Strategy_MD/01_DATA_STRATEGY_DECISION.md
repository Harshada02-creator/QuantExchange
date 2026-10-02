# 01 — Data Strategy Decision

## Objective

Build a realistic AI/quant market-intelligence layer on top of the frozen QuantExchange execution core.

## Recommended zero-cost strategy

### DATASET A — NSE
Daily India-focused quant baseline.

The intended data includes fields appropriate to:
- OHLC
- volume
- turnover where available
- returns
- volatility
- regime analysis

It is NOT an order-book/L3 dataset.
It is NOT suitable for order-level replay.

### DATASET B — IEX TOPS
Small intraday microstructure pilot.

Use for:
- best bid/ask
- aggregated bid/ask size
- last sales
- spread
- mid-price
- top-of-book imbalance
- trade intensity
- short-horizon return/volatility experiments

Important:
- IEX is a single venue.
- It is not consolidated U.S. market data.
- HIST is free.
- Current IEX pages say HIST is available T+1 and the most recent twelve months are available.
- Recent raw files can be multi-gigabyte and are exchange-wide.

Start with **one trading day and 3–5 symbols**, not months of raw files.

### DATASET C — FI-2010
Academic LOB benchmark.

Use for:
- LOB model benchmarking
- comparison with published research
- reproducible experiments

Do not use it as current Indian/U.S. market data or as raw order-by-order replay.

### FUTURE L3 DATA
Nasdaq ITCH / Databento / LOBSTER etc. deferred.

Candidates:
- Nasdaq public ITCH samples
- Databento XNAS.ITCH MBO
- LOBSTER

Use only when the project reaches order-by-order replay. Do not pay for these yet.

## Not the initial primary dataset

### Alpaca
Technically attractive for symbol-level historical/live data, but data-use/storage/redistribution terms must be checked before making it the persistent research source.

### Binance
Potentially useful for a later crypto high-frequency track, but it changes the market domain from equities to crypto. Keep optional.

### Paid NSE order/trade data
Not required for the initial zero-budget research track.

## Selection rule

Choose datasets by:
- technical fit
- historical span
- granularity
- license
- cost
- reproducibility
- processing size
- compatibility with the research goal
