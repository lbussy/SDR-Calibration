# Bounded in-memory acquisition execution prompt

## Objective

Implement the first code slice of the accepted live-device calibration
contract: a SoapySDR-independent component that acquires one exact bounded
complex-sample observation into memory, retains reviewable stream and cleanup
evidence, and returns samples only after complete acquisition and a known-safe
source state.

## Scope and requirements

1. Add the component under `src/capture/` using the existing `CapturePlan` and
   `SampleSource` abstractions. Do not depend on SoapySDR, Qt, filesystem output,
   network access, or external formats.
2. Enforce an explicit memory-byte ceiling before reserving storage or reading.
   Reject overflow, zero or inconsistent targets, invalid timeout limits, and a
   target that exceeds either capture-plan or memory limits.
3. Request no more samples than remain. Accept short reads while continuing to
   the exact bound and retain target, accepted count, read calls, short reads,
   timeouts, overflows, discontinuities, and available timestamps.
4. Reject empty sample reads, oversized reads, non-finite samples, timeout-limit
   exhaustion, overflow, discontinuity, early end, source errors, and thrown
   source exceptions. Check cancellation before allocation and between reads.
5. Attempt source cleanup exactly once on every path. Success requires complete
   acquisition, successful cleanup, and an explicitly `known_safe` final state.
   Failed, partial, cancelled, or unsafe results must expose no sample buffer.
6. Add deterministic hardware-free unit tests covering success, short reads,
   exact read bounds, preflight limits, non-finite samples, timeouts,
   discontinuity, cancellation, source exceptions, and cleanup failure or
   unknown state.
7. Update implementation status and the live-device roadmap without claiming a
   signal-quality analyzer, Soapy workflow boundary, production live mode,
   physical-device support, or calibration qualification.

## Non-goals and safety boundary

- Do not modify the production CLI, GUI, SoapySDR adapter, profile schema,
  packaging, trust management, or external services.
- Do not enumerate, open, configure, or stream from an SDR or USB device.
- Do not derive SNR, clipping, instability, interference, or reference
  suitability in this slice.
- Do not publish raw-IQ files or calibration artifacts.

## Validation and adversarial review

Run the narrow capture unit tests, the default hardware-free configure/build/
CTest baseline, the SoapySDR-disabled build and tests when supported by current
presets, Sphinx with warnings as errors, and `git diff --check`. Review the API
for allocation overflow, cleanup omission, partial-buffer leakage, read-bound
violations, exception escape, false success, and documentation overclaim.

Treat each actionable finding as a blocker, correct it, repeat affected checks,
and reassess until no actionable finding remains.

## Exit criteria

The component is deterministic, hardware-free, memory-bounded, cancellation-
aware, exception-contained, and fail-closed on incomplete acquisition or unsafe
cleanup. All applicable validation passes, and later live-device gates remain
explicitly planned.
