# QuantExchange — Local Verification Commands

## MSYS2 UCRT64 baseline

From the project root:

```bash
cd /c/Users/Shraddha/Desktop/QuantExchange
```

## Current direct compiler build

Main program:

```bash
g++ -std=c++14 -Wall -Wextra -Wpedantic -Iinclude src/Order.cpp src/Trade.cpp src/OrderBook.cpp src/MatchingEngine.cpp main.cpp -o QuantExchange.exe
```

Run:

```bash
./QuantExchange.exe
```

Tests:

```bash
g++ -std=c++14 -Wall -Wextra -Wpedantic -Iinclude src/Order.cpp src/Trade.cpp src/OrderBook.cpp src/MatchingEngine.cpp tests/MatchingEngineTests.cpp -o MatchingEngineTests.exe
```

Run:

```bash
./MatchingEngineTests.exe
```

## Verification rule

Capture the actual output.

A test count is verified only if the executable ran locally and printed passing results.
