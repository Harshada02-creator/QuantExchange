# QuantExchange — Testing and Acceptance Specification

## Principle

Correctness before optimization.

A feature is not complete because code was generated. It is complete only after it builds and its tests execute successfully.

## Existing verified tests

1. Order lifecycle.
2. Full match.
3. Partial fill.
4. No match.
5. FIFO priority.
6. Higher bid priority.
7. Multiple price levels.
8. Symbol isolation.
9. Duplicate order IDs.
10. Invalid order validation.
11. Successful BUY cancellation.
12. Successful SELL cancellation.
13. Cancellation of a partially filled order.
14. Cancellation of an unknown order.
15. Cancellation of an already filled order.
16. Cancellation of an already cancelled order.
17. Cancellation using the wrong symbol.
18. FIFO behavior after cancelling an earlier order.
19. Removal of an empty price level.

## Market-order acceptance tests

The market-order implementation should include tests for:

1. Market BUY against one ask level.
2. Market SELL against one bid level.
3. Market BUY sweeping multiple ask levels.
4. Market SELL sweeping multiple bid levels.
5. Market order fully filled.
6. Market order partially filled because liquidity runs out.
7. Market order with zero opposing liquidity.
8. Market order never rests on book.
9. Correct execution price per resting level.
10. FIFO behavior when consuming equal-price resting orders.
11. Limit behavior remains unchanged.
12. Cancellation behavior remains unchanged.

## Risk-engine acceptance tests later

Examples:
- insufficient cash rejected;
- maximum order quantity enforced;
- position limit enforced;
- projected exposure limit enforced;
- rejection occurs before matching;
- valid order passes through to matching.

## Persistence acceptance later

Examples:
- order records written correctly;
- trade records match engine output;
- positions update correctly;
- audit records are consistent;
- a database failure does not corrupt in-memory matching state in the documented design.

## Recovery acceptance later

Examples:
- checkpoint/event log exists;
- restart can reconstruct active book;
- replay is deterministic;
- rebuilt state matches pre-restart state.

## Performance acceptance later

Every benchmark must record:
- machine/environment;
- compiler/build mode;
- workload size;
- workload generation method;
- metric definition;
- measured result.

Never invent throughput or latency numbers.
