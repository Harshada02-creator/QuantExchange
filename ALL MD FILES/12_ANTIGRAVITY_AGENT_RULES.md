# QuantExchange — Antigravity Agent Rules

## Rule 1 — Read before modifying

Always inspect the current source tree and relevant tests before editing.

## Rule 2 — Establish baseline

Before a non-trivial change:
- build;
- run existing tests;
- record baseline.

If the environment cannot run them, say so explicitly.

## Rule 3 — One milestone at a time

Do not implement several future milestones in one request unless explicitly asked.

## Rule 4 — Preserve working behavior

Existing passing tests are contractual behavior unless a requirement explicitly changes them.

## Rule 5 — Minimal coherent change

Prefer the smallest change that satisfies the task.

## Rule 6 — No fake verification

Never write "tests passed" unless the tests actually executed and produced passing results in the reported environment.

Do not say "expected 29/29" in place of actual execution.

## Rule 7 — No fabricated performance

Never invent benchmark results.

## Rule 8 — No accidental architecture expansion

Do not add Kafka, PostgreSQL, React, Docker, Redis, APIs, concurrency, or AI just because they are available on the roadmap.

## Rule 9 — Explain risky changes

When an API must change, state:
- old signature;
- new signature;
- compatibility impact;
- tests affected;
- why the change is necessary.

## Rule 10 — Verify after modification

After implementation:
- build;
- run old tests;
- run new tests;
- run demo/smoke test;
- inspect failures;
- report exact results.

## Rule 11 — Safe object lifetime

Never return dangling references or pointers to erased orders.

## Rule 12 — Preserve determinism

Do not parallelize the core matching decision for a symbol without a documented deterministic sequencing design.

## Rule 13 — Stop at the requested milestone

Once the requested milestone is implemented and verified, stop.

## Rule 14 — Leave an audit trail

Summarize:
- files changed;
- behavior added;
- tests added;
- tests run;
- build command;
- test command;
- result;
- known limitations.
