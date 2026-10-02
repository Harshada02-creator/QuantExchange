# QuantExchange — Performance and Systems Engineering Target

## Performance philosophy

Do not optimize before correctness is established.

Do not add concurrency solely to make the project look advanced. The core matching decision for a symbol should remain deterministic.

## Metrics

### Throughput
Orders processed per second.

### Average latency
Mean order-processing time.

### P95 latency
95% of requests complete below the reported value.

### P99 latency
99% of requests complete below the reported value.

### Memory
Peak and/or steady-state memory for clearly defined workloads.

### Error/rejection rate
Failures or rejected orders, with reason categories.

## Workloads

Start with progressively larger workloads:
- 100,000 orders;
- 1,000,000 orders;
- larger stress case when the environment supports it.

Use repeatable random seeds where randomness is involved.

## Optimization topics for later

Only after profiling:
- allocation behavior;
- object pools;
- compact representations;
- lock contention;
- thread-safe queues for downstream work;
- serialization cost;
- separation of serialized matching from parallel downstream analytics.

## Evidence standard

A resume/interview claim should be traceable to:
- benchmark code;
- recorded workload;
- measured output;
- documented environment;
- reproducible command.
