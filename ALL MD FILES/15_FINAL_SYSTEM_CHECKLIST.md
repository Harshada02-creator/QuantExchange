# QuantExchange — Final System Checklist

## Core engine
- [ ] Limit orders match correctly.
- [ ] Market orders sweep correctly.
- [ ] Full fills work.
- [ ] Partial fills work.
- [ ] Cancellation works.
- [ ] FIFO works.
- [ ] Best price priority works.
- [ ] Multiple levels work.
- [ ] Symbol isolation works.

## Lifecycle
- [ ] New state correct.
- [ ] Accepted state correct.
- [ ] Partial fill state correct.
- [ ] Filled state correct.
- [ ] Cancelled state correct.
- [ ] Rejected state correct.
- [ ] Invalid transitions rejected.

## Risk
- [ ] Cash check.
- [ ] Holdings check.
- [ ] Position limit.
- [ ] Order size limit.
- [ ] Exposure limit.
- [ ] Configurable thresholds.
- [ ] Risk happens before matching.

## Persistence
- [ ] Orders.
- [ ] Trades.
- [ ] Positions.
- [ ] Audit records.
- [ ] Recovery design documented.

## Events/API/UI
- [ ] Event schemas.
- [ ] REST/API operations.
- [ ] WebSocket/streaming where used.
- [ ] Dashboard is a view over the real backend.

## Simulation/analytics
- [ ] Synthetic traders.
- [ ] Reproducible seeds.
- [ ] Spread/volume/imbalance/volatility/fill analytics.

## Performance
- [ ] Throughput benchmark.
- [ ] Average latency.
- [ ] P95.
- [ ] P99.
- [ ] Memory.
- [ ] Reproducible workloads.
- [ ] Results measured rather than invented.

## Engineering quality
- [ ] Clean C++ architecture.
- [ ] No dangling references.
- [ ] Strong tests.
- [ ] Build documented.
- [ ] Source comments explain non-obvious decisions.
- [ ] README is current.
- [ ] Interview questions can be answered from the implementation.
