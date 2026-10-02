# QuantExchange — Domain Rules and Examples

## Limit order example

SELL:
3405 -> 100
3404 -> 50
3402 -> 150

BUY:
3399 -> 100
3398 -> 80
3395 -> 200

Incoming BUY 100 @ 3402:
- best ask = 3402;
- buyer is willing to pay 3402;
- execute 100 @ 3402;
- seller remainder = 50 @ 3402;
- incoming remainder = 0.

## Partial fill example

BUY 100 @ 100
SELL 50 @ 100

Result:
- trade 50 @ 100;
- buy remaining 50;
- sell remaining 0.

## FIFO example

BUY A: 100 @ 100
BUY B: 100 @ 100

SELL 150 @ 100 must produce:
- 100 against A;
- 50 against B.

## Higher bid example

BUY A: 10 @ 99
BUY B: 10 @ 101

SELL 5 @ 99 should hit B first because 101 is the best bid.

## Market buy sweep

SELL:
101 -> 40
102 -> 30
103 -> 50

BUY 100 MARKET:
- 40 @ 101
- 30 @ 102
- 30 @ 103

Remainder at 103 = 20.

## Market order with insufficient liquidity

SELL:
101 -> 40
102 -> 30

BUY 100 MARKET:
- 40 @ 101
- 30 @ 102
- remaining 30 is cancelled;
- no part of the market order rests on the book.

## Market order with no liquidity

BUY 100 MARKET with no sell liquidity:
- zero trades;
- full 100 remains unfilled;
- order is cancelled;
- book is unchanged.

## Cancellation example

BUY A: 10 @ 100
BUY B: 10 @ 100
BUY C: 10 @ 100

Cancel B.

Remaining FIFO queue:
A, C

Canceling B must not reorder A and C.
