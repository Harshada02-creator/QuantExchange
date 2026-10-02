# 01 — Data Strategy Decision

## Objective

Build a realistic AI/quant market-intelligence layer on top of the frozen QuantExchange execution core.

## Recommended zero-cost strategy

### Dataset A — NSE public historical research data
**Role:** India-focused daily baseline.

Use for:
- daily returns
- volatility
- momentum
- volume behavior
- regime detection
- daily backtesting

Do **not** claim it provides bid/ask spread, intraday order flow, full order-book imbalance, or order-level replay.

### Dataset B — IEX HIST TOPS
**Role:** small intraday microstructure pilot.

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

### Dataset C — FI-2010
**Role:** academic benchmark.

Use for:
- LOB model benchmarking
- comparison with published research
- reproducible experiments

Do not use it as current Indian/U.S. market data or as raw order-by-order replay.

### Dataset D — future advanced L3 source
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

## Role map

| Role | Source | First use |
|---|---|---|
| India daily quant | NSE public research data | Daily models |
| Intraday microstructure pilot | IEX TOPS | Quotes/trades/features |
| Academic LOB benchmark | FI-2010 | Benchmarking |
| Advanced L3 replay | Nasdaq ITCH / Databento / LOBSTER | Later |
| Live streaming | TBD after licensing review | Much later |

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
