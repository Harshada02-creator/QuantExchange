# 06 — Data Pipeline, Versioning, and Evaluation

## Pipeline

### Daily India data

official source
→ raw staging
→ schema validation
→ normalization
→ corporate-action checks
→ version hash
→ PostgreSQL/Parquet
→ features

### IEX microstructure pilot

PCAP
→ decoder
→ symbol filter
→ normalized event stream
→ Parquet
→ features

### FI-2010

download
→ verify metadata/checksum
→ benchmark-specific preprocessing
→ reproducible evaluation

## Data-quality checks

All sources should check:
- required columns
- timestamp parseability
- timezone consistency
- symbol validity
- duplicate records
- impossible prices
- impossible OHLC relationships
- invalid quantities
- out-of-order events
- suspicious gaps

## Leakage prevention

Use chronological splits.

TRAIN = earliest period
VALIDATION = later period
TEST/HOLDOUT = latest untouched period

Do not randomly shuffle future and past observations for final evaluation.

Every feature at time t must use only information available by t.

## Walk-forward evaluation

For serious models:

train on past
→ validate on next window
→ move forward
→ repeat

Report:
- performance by window
- performance by regime
- transaction costs
- slippage
- turnover
- drawdown
- stability

## Dataset versioning

Record:
- source
- access/download date
- date range
- instruments
- raw-file names
- file hashes
- parser version
- schema version
- cleaning version
- feature version

## Data publication rule

Never commit raw vendor/exchange files to GitHub unless the exact license explicitly permits redistribution.
Publish code, schemas, fixtures and allowed derived results instead.
