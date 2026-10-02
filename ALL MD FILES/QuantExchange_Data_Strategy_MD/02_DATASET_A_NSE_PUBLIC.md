# 02 — Dataset A: NSE Public Historical Research Data

## Role

DATASET A — NSE
Daily India-focused quant baseline.

## Appropriate uses

The intended data includes fields appropriate to:
- OHLC
- volume
- turnover where available
- returns
- volatility
- regime analysis
- daily backtesting

## Not appropriate as the only source for

It is NOT an order-book/L3 dataset.
It is NOT suitable for order-level replay.
Do not use for:
- bid/ask spread
- intraday order imbalance
- order-book reconstruction

## Current Project Status

- The project has NOT yet ingested real NSE data.
- The previous fake NSE dataset was synthetic and has been permanently removed.
- The old fake provenance hash `74470f15696e29fa` must never be used again.

## Current official sources (access checked 2026-10-02)

The CURRENT official NSE Bhavcopy route is:
`CM-UDiFF Common Bhavcopy Final (zip)`

The old routes `CM - Bhavcopy(csv)` and `CM - Common Bhavcopy (csv)` were discontinued effective July 08, 2024.
Do NOT document guessed URLs such as `sec_bhavdata_full_<date>.csv` as the primary current ingestion route. The actual file URL must only be recorded after it is verified from the current official NSE site.

NSE Data Sharing & Usage Policy:
https://www.nseindia.com/static/market-data/nse-data-policy
https://www.nseindia.com/static/research/research-initiatives
https://www.nseindia.com/resources/historical-reports-capital-market-daily-monthly-archives
https://www.nseindia.com/historical/price-and-volume-data-per-security

NSE recognizes students/researchers as non-commercial users. Exact use restrictions are governed by the applicable policy, documentation, undertaking, or agreement. Prefer official downloads. Do not scrape blindly.

## First proposed universe

Start with ONE REAL NSE FILE.
Do not perform a bulk historical download at this stage.

## Data-quality requirements

Check:
- duplicate symbol/date rows
- invalid OHLC relationships
- missing dates
- zero/negative values
- holiday/weekend contamination
- symbol changes
- corporate-action issues where available

## Reproducibility

Store:
- source URL
- download date
- raw filename
- source checksum if possible
- parser version
- schema version
- dataset version hash
