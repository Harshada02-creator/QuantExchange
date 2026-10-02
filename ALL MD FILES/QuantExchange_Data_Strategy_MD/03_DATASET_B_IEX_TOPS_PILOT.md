# 03 — Dataset B: IEX HIST TOPS Pilot

## Role

A small, real, free intraday microstructure pilot.

## Why TOPS

TOPS provides top-of-book quotations and last-sale data.

Useful fields/concepts:
- best bid
- best ask
- aggregated bid size
- aggregated ask size
- trade price
- trade size
- event timestamp

Derived features:
- spread
- mid-price
- spread in basis points
- top-of-book imbalance
- trade intensity
- short-horizon return
- intraday volatility

## Important limitations

IEX is a single U.S. exchange venue. Do not describe IEX as consolidated U.S. market data, NBBO, or a complete representation of U.S. trading.

TOPS does not expose the number/size of individual orders at the best price.

## File-size reality

The official IEX market-data download page currently shows recent daily files in the multi-gigabyte range; it lists a 2026-09-16 TOPS file at 13.42 GB.

Therefore:

**Do not plan to download months of raw TOPS data at the start.**

## Pilot configuration

Start with:
- 1 trading day
- 3–5 liquid U.S. symbols
- TOPS only

IEX files are exchange-wide, so symbol filtering happens after download/stream processing.

## Processing strategy

raw compressed PCAP
→ streaming decoder
→ filter chosen symbols
→ normalized event table
→ Parquet
→ PostgreSQL metadata

Do not store the entire raw file uncompressed unless justified.

## Current official sources (access checked 2026-10-02)

IEX Market Data & Connectivity:
https://www.iex.io/products/equities/market-data-connectivity

IEX HIST download:
https://iextrading.com/trading/market-data/

IEX HIST terms:
https://www.iex.io/legal/hist-data-terms

IEX market-data resources:
https://www.iex.io/resources/trading/market-data

## Retention note

The current IEX product page says HIST provides free T+1 downloads and that the most recent twelve months are available.

Do not rely on third-party claims of a much older archive unless verified against a current primary source.

## License note

IEX's HIST terms say attribution is required when distributing/providing access and IEX retains proprietary rights.

For this project:
- do not publish raw IEX files
- do not commit them to GitHub
- preserve provenance/licence metadata
