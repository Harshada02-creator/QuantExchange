# 05 — Future L3, Replay, and Live Data

This file is a **future evaluation list**, not a download order.

## A. Nasdaq ITCH public samples

Role:
- parser development
- deterministic replay
- engine validation

Official sample directory:
https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/

Licensing/redistribution must be verified before publication or redistribution.

## B. Databento XNAS.ITCH MBO

Role:
- order-by-order Nasdaq replay
- order-ID experiments
- book-state comparison against QuantExchange

Official:
https://databento.com/catalog/xnas-itch
https://databento.com/pricing

Important:
- paid after promotional credits
- venue-specific licensing must be checked
- archive raw downloads and record retrieval date/version
- Databento has announced retrospective changes to historical XNAS.ITCH data for 2026; reproducibility requires raw-file/version archiving

## C. LOBSTER

Role:
- reconstructed Nasdaq order-book research
- deeper L3/L2 experiments
- advanced model/replay work

Official:
https://data.lobsterdata.com/
https://pilot.lobsterdata.com/

Important:
- commercial/academic service, not the same as FI-2010's open benchmark model
- university/student access arrangements may apply
- current pricing must be checked before spending money
- reconstruction/hidden-order rules must be documented for replay experiments

## D. Alpaca

Role:
- future symbol-level historical/live prototyping
- possible future paper-trading data integration

Official:
https://docs.alpaca.markets/docs/about-market-data-api

Important:
- Basic real-time equities data is IEX-only
- plan limits change
- data storage/redistribution rules must be checked before making it the persistent/public research source

## E. Binance public data

Role:
- optional crypto high-frequency research

Official:
https://data.binance.vision/
https://github.com/binance/binance-public-data

Do not add it to the primary equity strategy unless a crypto research track is intentionally created.

## Priority

1. Nasdaq ITCH sample for tiny replay development
2. Databento credit for a controlled MBO sample, if needed
3. LOBSTER only if a real research need justifies it
4. Live feed only after licensing/storage is clear
