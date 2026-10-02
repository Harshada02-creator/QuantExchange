# 07 — Phase 2 Execution Plan

## Step 0 — Freeze dataset specification

Record:
- source
- role
- fields
- instruments
- period
- license
- storage
- expected size

## Step 1 — NSE baseline

Acquire a small official public sample.

Validate:
- schema
- dates
- symbols
- OHLC consistency
- missingness

Then expand only if the archive and usage terms support it.

## Step 2 — IEX pilot

Acquire one day of TOPS.

Filter 3–5 chosen symbols during parsing.

Produce:
- normalized quotes
- normalized trades
- spread
- mid-price
- top-of-book imbalance

Confirm actual storage size before scaling.

## Step 3 — FI-2010 benchmark

Load FI-2010 as a separate benchmark dataset.

## Step 4 — Data-quality report

Report:
- rows
- date span
- symbols
- missingness
- duplicates
- anomalies
- file size
- provenance
- license notes

## Step 5 — Freeze feature interface

Define a source-independent feature contract.

Example outputs:
- return_1
- return_5
- realized_vol
- spread
- imbalance
- trade_intensity

## Step 6 — Only then model

Do not start serious model training until:
- source verified
- splits defined
- leakage checks passed
- features reproducible
- baseline metrics defined
