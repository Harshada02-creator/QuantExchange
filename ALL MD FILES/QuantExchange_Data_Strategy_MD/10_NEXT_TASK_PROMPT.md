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
- exchange core frozen
- 41/41 exchange tests passed
- Data Foundation Step 1 implemented
- Python validation pipeline verified

CURRENT TASK:
Acquire and ingest a SMALL real NSE public historical research-data sample.

Do NOT:
- download the full dataset
- download IEX yet
- train ML models
- build features yet
- modify the frozen C++ exchange core

First:
1. Identify the exact official NSE public/research dataset and official download page.
2. Record source URL, access date, fields, date range, symbol universe, and applicable usage conditions.
3. Download only a small manual sample.
4. Run it through the existing intelligence ingestion/validation pipeline.
5. Store normalized rows in the existing PostgreSQL data layer.
6. Produce a data-quality summary.
7. Record dataset provenance/version/hash.

Then run:
- all existing C++ regression tests
- all Python data-foundation tests

Report only actual results.
If access/usage terms are ambiguous, STOP and report the ambiguity instead of downloading at scale.

STOP after the small real NSE sample is successfully validated and ingested.
```
