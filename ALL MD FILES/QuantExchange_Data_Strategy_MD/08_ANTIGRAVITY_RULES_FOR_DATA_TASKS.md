# 08 — Antigravity Rules for Data Tasks

## Before every data-related task

Read the core QuantExchange master context SIDE-BY-SIDE with the current repository, then read these data files:
- 00_READ_ME_FIRST.md
- 01_DATA_STRATEGY_DECISION.md
- 02_DATASET_A_NSE_PUBLIC.md
- 03_DATASET_B_IEX_TOPS_PILOT.md
- 04_DATASET_C_FI2010.md
- 05_FUTURE_L3_AND_LIVE_DATA.md
- 06_DATA_PIPELINE_AND_SPLITS.md
- 07_PHASE_2_EXECUTION_PLAN.md
- 09_MASTER_CURRENT_DATASET_PLAN.md
- 10_NEXT_TASK_PROMPT.md

## Hard rules

1. Do not modify the frozen C++ exchange core during data tasks unless explicitly required.
2. Do not download a large dataset without reporting estimated storage first. No bulk historical download at this stage.
3. Do not treat a public URL as permission to redistribute data.
4. Read the actual license/terms.
5. Never commit raw market data to Git.
6. Keep provenance for every dataset.
7. Separate raw, normalized and derived data.
8. Never claim ML validity without leakage-safe evaluation.
9. Never call single-venue data "the market".
10. Never state an unverified licensing claim as fact.
11. Run exchange regression tests after repository changes.
12. Report only tests actually executed.

## Current objective

Build:
NSE daily baseline
+ IEX intraday pilot
+ FI-2010 benchmark
→ feature engine
→ quant models
→ backtesting
→ AI assistant
→ paper trading

Do not jump from a CSV to a generic "best trade" chatbot.
