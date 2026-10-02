# QuantExchange — Definition of Done

A feature is DONE only when all applicable conditions are true.

## Functional
- Requirement is implemented.
- Normal path works.
- Edge cases are covered.
- Invalid states are rejected safely.
- Existing features remain correct.

## Correctness
- Invariants remain true.
- No duplicate active order IDs.
- No impossible quantities.
- No stale active orders after fill/cancel.
- Symbol isolation is preserved.
- Deterministic ordering is preserved.

## Tests
- New focused tests exist.
- Existing regression tests pass.
- Full test suite passes.

## Build
- Project compiles with the documented toolchain.
- Warnings are reviewed.
- No untracked generated artifacts are required for a clean build unless documented.

## Documentation
- Behavior is explained.
- Any API change is documented.
- Limitations are documented.

## Verification
- Test commands are recorded.
- Actual output is known.
- "Verified" means executed, not inferred.

## Performance
- Claims are backed by reproducible measurements.

## Interview readiness
- The developer can explain the feature from first principles.
