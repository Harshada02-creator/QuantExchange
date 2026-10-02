# QuantExchange — Code Structure and Engineering Rules

## Expected project structure

QuantExchange/
|
+-- include/
|   +-- Order.h
|   +-- Trade.h
|   +-- OrderBook.h
|   +-- MatchingEngine.h
|
+-- src/
|   +-- Order.cpp
|   +-- Trade.cpp
|   +-- OrderBook.cpp
|   +-- MatchingEngine.cpp
|
+-- tests/
|   +-- MatchingEngineTests.cpp
|
+-- main.cpp
+-- CMakeLists.txt
+-- documentation / markdown context files (outside core source tree as desired)

## Current C++ standard

C++14.

Do not silently migrate to C++17/20 unless explicitly planned and all compatibility consequences are documented.

## Price representation

The current design stores prices as integer smallest units (for example paise) rather than floating point values. Preserve deterministic comparisons.

## Error handling

Invalid lifecycle transitions and invalid inputs should fail explicitly using the current exception/error style unless the project later adopts a documented result-based error model.

Never hide failures.

## Memory/reference safety

Never return a dangling pointer/reference to an order that has been erased from the book.

When removal is required and the caller needs final order state, return a safe copy/result object.

## Ownership

The MatchingEngine owns the set of symbol books.
The OrderBook owns active resting orders.
Trade objects returned to callers should be independent of the mutable book state.

## ID rules

Order IDs are globally unique within the engine session.
Trade IDs must also be non-zero and unique within the engine session.

## Symbol isolation

TCS orders must never match/cancel against INFY orders.

## Build hygiene

Source files should explicitly include the standard headers for the functionality they use.
Avoid relying on transitive includes.

## Change discipline

Before modifying code:
1. Read existing code.
2. Identify invariants.
3. Run baseline tests if possible.
4. Make the smallest coherent change.
5. Add focused tests.
6. Run all old and new tests.
7. Report exact results.

Do not refactor unrelated code during feature work without a concrete reason.
