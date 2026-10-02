# QuantExchange — Architecture and System Flow

## 1. Target high-level architecture

Trader Clients
    |
    v
Order Gateway
    |
    v
Validation
    |
    v
Pre-Trade Risk Engine
    |
    v
Matching Engine (C++)
    |
    +--> Order Book
    |
    +--> Trade / Order Events
             |
             +--> Event Stream (advanced)
             |        |
             |        +--> Persistence
             |        +--> Analytics
             |        +--> Market Data
             |
             +--> APIs / WebSocket
                      |
                      v
                  Dashboard

## 2. Current core flow

Input order
-> Order object
-> submit to MatchingEngine
-> identify symbol OrderBook
-> validate order state / unique ID
-> accept order
-> inspect opposite-side best order
-> determine whether the order can match
-> calculate fill quantity
-> generate Trade
-> update both order states/remaining quantities
-> remove fully filled resting orders
-> continue if incoming order has remaining quantity
-> rest valid limit remainder OR cancel market remainder
-> return observable results.

## 3. Current core components

### Order
Represents a trader order and its lifecycle state.

### Trade
Represents one execution event between a buy and sell order.

### OrderBook
One book per symbol. Maintains:
- buy price levels in descending order;
- sell price levels in ascending order;
- FIFO order queues at each price level;
- active order ID tracking.

### MatchingEngine
Owns books by symbol, owns trade ID sequencing, accepts orders, applies matching rules, and handles cancellation.

## 4. Data structures

Current intended core representation:

BUY:
map<Price, deque<Order>, greater<Price>>

SELL:
map<Price, deque<Order>, less<Price>>

This gives ordered price levels plus FIFO queues.

## 5. Why the matching core stays deterministic

For a given symbol, matching decisions should have a deterministic sequence. Do not introduce multithreading into the core just to claim parallelism.

Independent downstream work can later become asynchronous:
- persistence;
- analytics;
- dashboard updates;
- event consumers.

## 6. Layering rule

Build in this order:
1. Correct C++ matching core.
2. Lifecycle and risk.
3. Persistence.
4. Services/API.
5. Event streaming.
6. Dashboard/analytics.
7. Performance and advanced recovery.

Do not reverse this order by making the UI the project and leaving the backend trivial.
