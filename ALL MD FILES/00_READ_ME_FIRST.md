# QuantExchange — READ ME FIRST

## Purpose of this pack

This folder is the master context pack for continuing development of **QuantExchange — High-Performance Electronic Trading & Market Simulation Engine**.

Antigravity must read this file first, then read the remaining Markdown files in numeric order before modifying the project.

## Non-negotiable principle

Build the **correct, deterministic C++ matching core first**. Surround it with persistence, APIs, streaming, analytics, UI, and performance infrastructure only after the core is correct and tested.

The project is an **educational exchange simulator**. It does not execute real-money trades, connect to a live broker, or claim to reproduce every real-world exchange rule.

## Current verified status

### VERIFIED LOCALLY
- V1 limit-order matching engine: implemented and locally compiled.
- Main demo: locally compiled and executed successfully.
- Original test suite: 10/10 passed.
- Cancellation feature: implemented and locally verified.
- Cancellation test suite brought total to **19/19 passed**.

### IMPLEMENTED BUT NOT YET LOCALLY VERIFIED
- Market-order functionality was implemented by the coding agent.
- The coding agent reported an expected total of 29 tests, but it could not compile/run them in its sandbox.
- Therefore market-order behavior must be treated as **unverified until the user runs the code locally**.

Never convert an unverified claim into a verified project status.

## Immediate next task

Before adding another feature:

1. Inspect the current source tree.
2. Build the current project locally using the available compiler/toolchain.
3. Run the entire test suite.
4. Verify the market-order tests and existing 19 tests.
5. Fix only actual failures.
6. Report exact observed results.
7. Stop before starting the risk engine.

## What the final project should become

The target project is a coherent, explainable electronic exchange simulator with:

- Correct order model and lifecycle.
- Price-time-priority order books.
- Limit and market orders.
- Full and partial execution.
- Cancellation.
- Pre-trade risk controls.
- In-memory deterministic matching core.
- Persistent orders/trades/positions/audit records.
- Event-driven market/trade events.
- REST/WebSocket services.
- Interactive market dashboard.
- Synthetic traders and analytics.
- Reproducible performance benchmarks.
- Optional event replay/recovery.
- Strong automated tests.
- Documentation that explains design decisions and trade-offs.

## Agent behavior

Do not make unrelated changes, do not rewrite working architecture without evidence, do not invent benchmark numbers, and do not claim tests passed unless they actually ran.
