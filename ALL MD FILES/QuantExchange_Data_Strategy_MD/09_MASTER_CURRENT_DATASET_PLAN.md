# 09 — Master Current Dataset Plan

## Current roles

### A — NSE public research data
Purpose: India daily quant layer.
Status: **NEXT**

### B — IEX HIST TOPS
Purpose: small intraday microstructure pilot.
Status: **AFTER BASIC NSE PIPELINE WORKS**

### C — FI-2010
Purpose: academic benchmark.
Status: **BENCHMARK**

### D — L3/order replay
Purpose: advanced QuantExchange replay.
Status: **LATER**

## Immediate action

Do not download all datasets at once.

Sequence:
1. Confirm exact official NSE public dataset/file and access conditions.
2. Acquire a small NSE sample manually.
3. Ingest and validate it through the existing data-foundation pipeline.
4. Confirm size and data quality.
5. Run the IEX TOPS one-day pilot.
6. Load FI-2010.
7. Only then begin feature engineering.

## Intended ML families

### Daily
NSE daily data → return/volatility/regime research.

### Microstructure
IEX pilot + FI-2010 → short-horizon direction/return and microstructure research.

### Future execution model
L3/order data → fill probability/execution outcome.

## Non-goals

This phase does not attempt:
- live money trading
- brokerage execution
- production financial advice
- generic "best stock" chatbot
- giant foundation model

The future AI should explain evidence and scenarios, while paper trades flow through the already-built risk and matching engine.
