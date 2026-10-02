# 04 — Dataset C: FI-2010 Academic Benchmark

## Role

Open academic benchmark for short-horizon limit-order-book prediction.

## Use it for

- benchmarking model architectures
- comparing metrics with published research
- reproducible LOB experiments

## Do not use it for

- current Indian market data
- current U.S. market data
- raw order-by-order replay
- direct feeding into the QuantExchange matching engine

FI-2010 is a processed/normalized research benchmark.

## Commonly cited characteristics

- five Finnish stocks
- ten trading days in 2010
- roughly four million order-book events
- multiple book levels/features
- short-horizon prediction labels

## Official landing page used by the project

https://etsin.fairdata.fi/dataset/73eb48d7-4dbc-4a10-a52a-da745b47a649

Re-check the current Fairdata record/license immediately before public redistribution or final publication.

## Research workflow

dataset
→ reproducible preprocessing
→ chronological train/validation/test
→ model
→ metrics
→ comparison with published baselines

Keep FI-2010 separate from the real-market ingestion pipeline.
