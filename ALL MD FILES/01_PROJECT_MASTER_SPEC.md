# QuantExchange — Project Master Specification

## 1. Project title

**QuantExchange — High-Performance Electronic Trading & Market Simulation Engine**

## 2. Core objective

Build an educational simulation of an electronic exchange that receives, validates, prioritizes, matches, executes, cancels, records, and publishes simulated trading activity.

The project demonstrates the intersection of:

- Finance: orders, bids/asks, spreads, liquidity, execution, positions, exposure, risk limits.
- Algorithms: price-time priority, matching, partial fills, cancellation, efficient order-book operations.
- Systems: in-memory state, event-driven architecture, deterministic sequencing, recovery, performance measurement.
- Data: trade/order persistence, streaming events, market analytics, synthetic workloads.
- CS fundamentals: C++, DSA, OOP, DBMS, networking, testing, performance engineering.

## 3. Scope boundary

This is an educational simulator.

It must NOT:
- execute real-money trades;
- connect to a live brokerage by default;
- imply that it reproduces every rule of a real exchange;
- claim production exchange performance unless supported by measured benchmarks.

## 4. Central engineering objective

The strongest part of the project is the matching engine and its surrounding correctness story, not the dashboard.

The system must be understandable from first principles:
- what each component does;
- why it exists;
- how its data structures work;
- what invariants it preserves;
- what trade-offs were made;
- how behavior is tested;
- how performance is measured.

## 5. Core functional requirements

### Orders
- Unique order ID.
- Symbol/instrument.
- Side: Buy/Sell.
- Type: Limit/Market.
- Original quantity.
- Remaining quantity.
- Filled quantity.
- Price for limit orders.
- Lifecycle status.

### Order lifecycle
Recommended lifecycle:

NEW -> VALIDATED -> ACCEPTED -> MATCHING -> PARTIALLY_FILLED -> FILLED

Alternate paths:
- NEW -> REJECTED
- ACCEPTED/PARTIALLY_FILLED -> CANCELLED
- MARKET remainder -> CANCELLED when no further liquidity exists.

The concrete implementation may keep the current simplified states when they preserve semantics.

### Limit orders
- Buy limit executes against the best sell only when buy price >= best sell price.
- Sell limit executes against the best buy only when sell price <= best buy price.
- If a limit order has a remainder after all compatible liquidity is consumed, that remainder may rest on the book.

### Market orders
- Market Buy consumes lowest sell price first.
- Market Sell consumes highest buy price first.
- Continue across price levels until filled or opposite liquidity is exhausted.
- Execution price is the resting order price.
- A market order must never rest on the book in the current project design.
- If liquidity runs out, the remainder is cancelled.

### Price-time priority
BUY:
1. Higher price first.
2. Earlier arrival first at the same price.

SELL:
1. Lower price first.
2. Earlier arrival first at the same price.

### Partial fill
trade_quantity = min(incoming_remaining, resting_remaining)

After a partial fill:
- both orders' remaining quantities update correctly;
- status becomes PartiallyFilled where appropriate;
- a resting remainder stays at its existing FIFO position.

### Cancellation
- Identify instrument and order ID.
- Only active orders may be cancelled.
- Partially-filled orders may be cancelled.
- Cancellation removes the order from the price-level queue.
- FIFO order of unrelated orders must not change.
- Empty price levels must be removed.
- Cancellation generates no trade.
- Unknown, already filled, and already cancelled orders do not mutate the book.

## 6. Correctness invariants

The implementation should maintain these invariants:

- No active order has zero remaining quantity.
- A filled order has zero remaining quantity.
- Filled quantity + remaining quantity = original quantity.
- A cancelled order never remains active in the book.
- A filled order never remains active in the book.
- The same active order ID cannot exist twice.
- An order can only belong to its symbol's book.
- A market order never rests.
- Matching never crosses incompatible prices for limit orders.
- Trade quantities never exceed either order's remaining quantity.
- Trades always identify distinct buy and sell order IDs.
- Execution price comes from the resting order in the current matching model.
- FIFO at equal price is preserved except when an earlier order is removed/cancelled.

## 7. Quality goal

The project should be:
- correct;
- deterministic;
- testable;
- explainable;
- measurable;
- modular;
- progressively extensible;
- free from fabricated performance claims.
