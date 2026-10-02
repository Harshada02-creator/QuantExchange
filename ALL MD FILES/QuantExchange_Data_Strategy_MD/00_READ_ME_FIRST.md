# QuantExchange — Data Strategy Context Pack

## Purpose

This folder defines the **zero-cost / low-cost data strategy** for the AI/Quant Market Intelligence track of QuantExchange.

Read these files **alongside the existing QuantExchange master context and the current repository** before any dataset is downloaded or ingested.

## Current project state

The exchange foundation is frozen and regression-tested:
- C++ exchange engine
- Limit and market orders
- Price-time priority and FIFO
- Partial/full fills
- Cancellation
- Explicit lifecycle/state machine
- Pre-trade risk engine
- PostgreSQL persistence
- 41/41 core tests previously verified locally

The intelligence track is now separate.

## Critical decision

Do **not** search for one "perfect" dataset.

Use a role-based multi-dataset strategy:
1. **NSE public historical research data** — India-focused daily quant baseline.
2. **IEX HIST TOPS** — small real intraday microstructure pilot.
3. **FI-2010** — open academic LOB benchmark.
4. **Later: Nasdaq ITCH / Databento / LOBSTER** — advanced order-level replay.
5. **Later: live/streaming source** — only after license/storage terms are verified.

## Current rule

Do not download a large dataset yet.

First freeze:
- dataset role
- exact source
- exact fields
- dates
- symbols
- license/usage conditions
- storage plan
- train/validation/test strategy

Then acquire a small sample and validate the pipeline.
