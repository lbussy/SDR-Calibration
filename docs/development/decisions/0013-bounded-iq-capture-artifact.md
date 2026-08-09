# 0013 — Bounded complex-IQ capture artifact

Status: Accepted

Date: 2026-08-09

## Decision

Implement planned raw-IQ acquisition as reusable capture components behind the
SoapySDR boundary, with a thin SDR Calibration CLI. Keep the calibration core
independent of capture, SoapySDR, and filesystem paths.

The MVP artifact is a bounded, single-channel, headerless CF32 little-endian raw
file paired with a separately versioned JSON capture manifest. The manifest
preserves requested and effective settings, device and stream provenance,
sample and byte bounds, stream events, outcome, cleanup, and final device-state
knowledge. It is not an SDR Calibration Profile.

Partial data may be retained only when it has a truthful incomplete manifest and
cannot be mistaken for a complete artifact. SigMF, CS16, vendor-specific gain
control, DSP, visualization, resampling, compression, multi-channel capture,
network control, and device qualification are outside the MVP.

## Consequences

The capture loop, artifact writers, request/result types, and SoapySDR adapter
can be reused without the CLI. CF32 costs eight bytes per complex sample but
avoids an additional integer scaling contract. Every capture must therefore
have explicit time or sample bounds and an enforced byte bound.

Adopting another container or metadata standard later requires an adapter or a
new reviewed artifact version; it does not silently change version 1 captures.
