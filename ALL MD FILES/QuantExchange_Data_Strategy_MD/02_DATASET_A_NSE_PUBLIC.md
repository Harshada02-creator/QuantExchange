# 02 — Dataset A: NSE Public Historical Research Data

## Role

India-focused baseline for daily quant research.

## Appropriate uses

- OHLC / price research
- returns
- volatility
- momentum
- volume / turnover analysis
- regime detection
- daily backtesting

## Not appropriate as the only source for

- bid/ask spread
- intraday order imbalance
- order-book reconstruction
- order-level replay

## Important policy distinction

Do not mix:
1. public/research-oriented historical reports and data that may be available without cost under the research-data policy; and
2. paid/contractual historical Order & Trade products.

The zero-budget plan starts with the public research-data category.

## Current official sources (access checked 2026-10-02)

NSE Data Sharing & Usage Policy:
https://www.nseindia.com/static/market-data/nse-data-policy

NSE research initiatives:
https://www.nseindia.com/static/research/research-initiatives

NSE research-data classification:
https://nsearchives.nseindia.com/web/sites/default/files/inline-files/Data%20list%20under%20NSE%20Data%20Sharing%20Policy%20for%20Research%20and%20Analysis_20250728.pdf

NSE historical reports:
https://www.nseindia.com/resources/historical-reports-capital-market-daily-monthly-archives

NSE security-wise price/volume:
https://www.nseindia.com/historical/price-and-volume-data-per-security

## Current interpretation

NSE recognizes students/researchers as non-commercial users. Exact use restrictions are governed by the applicable policy, documentation, undertaking, or agreement.

NSE's current website materials also distinguish freely available research data from restricted or paid data categories.

Prefer official downloads. Do not scrape blindly.

## First proposed universe

Start with 10–20 liquid Indian equities.

Example candidates:
- TCS
- INFY
- RELIANCE
- HDFCBANK
- ICICIBANK
- SBIN
- ITC
- LT
- BHARTIARTL
- AXISBANK

Freeze the exact universe before bulk ingestion.

## First proposed period

Target multiple years if the freely accessible official archive actually provides that period. Do not promise a fixed five-year span until the exact archive is checked.

## Required normalized fields

At minimum:
- symbol
- trading_date
- open
- high
- low
- close
- volume

Add consistently available official fields where useful:
- turnover
- number of trades
- delivery quantities

## Data-quality requirements

Check:
- duplicate symbol/date rows
- invalid OHLC relationships
- missing dates
- zero/negative values
- holiday/weekend contamination
- symbol changes
- corporate-action issues where available
- survivorship-bias risks

## Reproducibility

Store:
- source URL
- download date
- raw filename
- source checksum if possible
- parser version
- schema version
- dataset version hash
