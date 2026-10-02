# 10 — Next Task Prompt for Antigravity

Copy this only after reviewing the dataset strategy:

```text
Before doing anything, read the QuantExchange master context SIDE-BY-SIDE with the current repository.

Then read:
- 00_READ_ME_FIRST.md
- 01_DATA_STRATEGY_DECISION.md
- 02_DATASET_A_NSE_PUBLIC.md
- 03_DATASET_B_IEX_TOPS_PILOT.md
- 04_DATASET_C_FI2010.md
- 05_FUTURE_L3_AND_LIVE_DATA.md
- 06_DATA_PIPELINE_AND_SPLITS.md
- 07_PHASE_2_EXECUTION_PLAN.md
- 08_ANTIGRAVITY_RULES_FOR_DATA_TASKS.md
- 09_MASTER_CURRENT_DATASET_PLAN.md

Also read the existing QuantExchange master context and inspect the current repository.

CURRENT VERIFIED STATE:
COMPLETED:
- exchange core
- risk
- lifecycle
- cancellation
- market orders
- PostgreSQL
- persistence verification
- test isolation
- data-foundation scaffold
- fake-data cleanup
- GitHub cleanup

NOT COMPLETED:
- real NSE ingestion
- IEX ingestion
- FI-2010 ingestion
- feature engineering
- ML/quant models
- AI market assistant
- paper trading product
- dashboard/API

NEXT ACTIVE DATA TASK:
Acquire ONE genuine NSE public historical market-data file from the official NSE source (`CM-UDiFF Common Bhavcopy Final (zip)`) and validate it through the existing data foundation.

Do NOT:
- download the full dataset or perform a bulk historical download
- download IEX yet
- train ML models
- build features yet
- modify the frozen C++ exchange core

First:
1. Obtain ONE real official NSE file.
2. Preserve raw file unchanged.
3. Record source/provenance.
4. Inspect schema.
5. Normalize.
6. Ingest.
7. Validate source-to-database values.
8. Run tests.
9. STOP.

Report only actual results.
If access/usage terms are ambiguous, STOP and report the ambiguity instead of downloading at scale.

STOP after the small real NSE sample is successfully validated and ingested.
```
