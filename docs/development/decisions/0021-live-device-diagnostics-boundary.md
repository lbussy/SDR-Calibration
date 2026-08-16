# 0021 — Live-device diagnostics boundary

Status: Accepted

Date: 2026-08-16

## Decision

Production live-device calibration will use the existing shared calibration
service through a separately implemented live workflow boundary. SoapySDR
remains behind the SDR boundary, and bounded in-memory acquisition and signal-
quality analysis remain independent, reviewable components.

Every observation-acceptance input must carry explicit provenance. Sample- and
stream-dependent diagnostics must be derived by versioned application code from
the exact bounded samples and session evidence. A request file or operator
assertion cannot substitute for application-derived SNR, clipping, missing-
sample, discontinuity, instability, or interference metrics. Reference
suitability comes from authenticated registry evidence; operator assertions
about reference conditions remain explicit and cannot alone create
qualification-grade acceptance.

Recorded-input and live-device modes remain schema-discriminated and may not
silently fall back to one another. Both modes use the same shared workflow and
artifact-publication semantics. Production live mode is available only through
the explicit schema after bounded acquisition, diagnostics derivation,
lifecycle mapping, and hardware-free integration review. Physical qualification
remains a separate gate.

The detailed contract is [live-device calibration](../live-device-calibration-contract.md).

## Consequences

The implementation proceeded through separate bounded in-memory acquisition,
signal-quality analysis, injected Soapy workflow, CLI, and GUI gates. Physical-
device qualification remains separate. This avoids trusting user-entered
assurance data and preserves the calibration core's independence from hardware
and user-interface concerns.

No source-only or fake-boundary result establishes device support, calibration
accuracy, or end-to-end qualification.
