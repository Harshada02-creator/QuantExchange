# QuantExchange — Documentation, Interview and Resume Outcome

## Documentation outcome

The repository should eventually explain:
- project goal;
- scope and non-scope;
- architecture;
- order lifecycle;
- data structures;
- matching algorithm;
- correctness invariants;
- risk rules;
- persistence/event design;
- API design;
- tests;
- benchmarks;
- trade-offs;
- limitations.

## Interview readiness

You should be able to explain, in your own words:

- Why C++ is used for the matching engine.
- How price-time priority works.
- Why the buy and sell books use opposite price ordering.
- Why FIFO is needed at equal price.
- How a partial fill works.
- Why execution price comes from the resting order in the current model.
- How cancellation preserves FIFO for unrelated orders.
- How market orders sweep multiple levels.
- Why market order remainders do not rest.
- Why risk is checked before matching.
- Why the engine keeps hot state in memory.
- Why every event need not be written synchronously to SQL in the hot path.
- How deterministic replay could rebuild the book.
- How benchmark methodology avoids fabricated numbers.
- How concurrency is separated from the serialized matching decision.

## Resume outcome

Only claim features that were actually built and verified.

Do not publish:
- fabricated throughput;
- fabricated latency;
- claims of live trading connectivity that do not exist;
- claims that the simulator implements all real-world exchange rules.

Possible final resume themes (after implementation and verification):
- C++ electronic exchange simulator.
- Price-time-priority order matching.
- Limit/market order execution.
- Cancellation and partial fills.
- Configurable pre-trade risk controls.
- Persistent trade/order records.
- Event-driven market data.
- Reproducible performance benchmarks.
