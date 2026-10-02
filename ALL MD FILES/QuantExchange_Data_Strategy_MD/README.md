# QuantExchange Data Strategy MD Pack

Use this pack to keep the dataset/intelligence track consistent.

Recommended immediate sequence:

**NSE public sample → validate pipeline → IEX TOPS one-day pilot → FI-2010 benchmark → features → quant models → backtesting → AI assistant → paper trading.**

Advanced L3 and live data come later.

The frozen exchange core remains untouched during data-foundation work unless a later integration task explicitly requires a controlled interface change.

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
