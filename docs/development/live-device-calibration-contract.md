# Live-device calibration contract

## Status and scope

This document freezes the contract for a planned production live-device
calibration path. Its bounded acquisition, signal-quality analyzer, and
injected fake-Soapy workflow boundary are implemented and hardware-free tested.
The production CLI has explicit live-request wiring; the GUI remains recorded-
input only, and no physical live combination is qualified.
The durable architectural boundary is accepted in
[decision 0021](decisions/0021-live-device-diagnostics-boundary.md).

The live path will connect the existing receive-only SoapySDR boundary to the
shared calibration workflow without moving SoapySDR into the calibration core.
It must produce the same native profile, evidence, summary, and optional lossy
WSJT-X projection as the recorded-input path. Input provenance and acquisition
lifecycle differ and must remain explicit.

## Required component boundary

The live path consists of five reviewable responsibilities:

1. A production adapter resolves one stable device selection, applies requested
   settings, reads effective settings, owns the RX stream, and guarantees
   reverse-order cleanup.
2. A bounded in-memory acquisition component reads one observation without
   publishing an intermediate file as a completed calibration artifact.
3. A signal-quality analyzer derives sample-dependent acceptance metrics and
   retains enough reduced evidence to reproduce or independently review them.
4. A live workflow boundary maps authenticated reference evidence, effective
   device state, samples, derived diagnostics, cancellation, and cleanup into
   the existing application-service contracts.
5. The production service publishes final artifacts only after every
   observation, model, uncertainty, assurance, profile, and evidence gate
   succeeds.

The calibration core remains independent of Qt, SoapySDR, operating-system
paths, network access, and external file formats. CLI and GUI clients must use
the same production service rather than implementing separate acquisition or
acceptance rules.

## Device identity and effective configuration

A live request must use explicit SoapySDR selection arguments that resolve to
exactly one device. Enumeration index is diagnostic input only and cannot be
stored as durable identity. Before streaming, the adapter must construct a
native device identity from stable driver and hardware attributes and compare
it with the request's expected identity.

The request and effective configuration remain separate. The adapter must
retain requested and read-back center frequency, sample rate, bandwidth, gain,
gain mode, frequency correction, clock source, antenna, tuner path, driver
version, firmware version, and resolved device arguments when available.
Strict policy rejects an explicitly requested setting that cannot be verified.
Any identity or effective-configuration change between observations fails the
run before model fitting.

Driver-applied frequency correction remains explicit. The calibration workflow
must not apply it a second time.

## Bounded acquisition lifecycle

Each observation requires a positive sample or duration bound and is subject to
the capture component's hard duration, sample, byte, timeout, and consecutive-
timeout ceilings. The in-memory component must additionally enforce an explicit
memory ceiling before device construction. It reads no more samples than remain
and rejects non-finite samples. The memory ceiling uses the implementation's
actual in-memory complex-sample size rather than the serialized CF32LE size.
The future live boundary must call the component's public preflight validation
before constructing a device, then apply the same validated limits to capture.

Cancellation is checked before discovery, before device construction, between
reads, before analysis, between observations, and before artifact publication.
Every path after construction attempts stream deactivation, stream close, and
device release in reverse order. Acquisition succeeds only when the requested
bound is complete and final device state is known safe. A partial, failed, or
cancelled observation is never eligible for estimation or fitting.

No completed raw-IQ artifact is published implicitly. Optional raw-IQ retention
is a separately requested expanded-evidence class governed by the existing
privacy and size limits. Reduced evidence is the default.

## Acceptance-input provenance

Every input to `ObservationDiagnostics` requires a recorded derivation:

| Diagnostic | Required provenance |
| --- | --- |
| Carrier estimate and drift | Derived from the exact bounded sample buffer by the versioned carrier estimator |
| Duration | Derived from effective sample rate and accepted sample count; wall-clock timing may be retained separately |
| Signal-to-noise ratio | Derived by a versioned signal-quality analyzer from the exact sample buffer |
| Clipped samples | Derived from samples using a documented, device-independent normalized clipping rule or a separately reviewed device-specific rule |
| Missing samples | Derived from expected versus accepted sample counts and stream evidence |
| Discontinuities | Derived from stream status and timestamp continuity evidence |
| Frequency instability | Derived from bounded-window departures from the accepted carrier model |
| Interference-to-carrier ratio | Derived by the versioned signal-quality analyzer from the exact sample buffer |
| Effective configuration | Derived from requested/effective readback and strict-policy evaluation |
| Reference suitability | Resolved from authenticated reference-registry evidence |
| Reference conditions met | Established from machine-verifiable evidence where possible; an operator assertion remains explicit and cannot alone create qualification-grade acceptance |

The version-one signal-quality algorithms, windowing, bin exclusion,
normalization, clipping definition, bounds, and fixture tolerances are frozen
and hardware-free reviewed. They produce acceptance diagnostics, not
measurement-uncertainty estimates. Physical-input behavior, threshold margins,
and any resulting uncertainty treatment remain qualification gates. Production
live calibration remains unavailable until the later workflow and client gates
are implemented. Request files must not supply these values as substitutes for
application derivation.

## Evidence and publication

Reduced evidence must bind each observation ID to device identity, requested
and effective configuration, UTC and monotonic timing, sample count, stream
statistics, estimator and analyzer versions, analyzer inputs and outputs,
acceptance policy and result, reference resolution, cancellation state, and
cleanup result. Raw samples remain opt-in.

The final transaction uses a new output directory and coordinated atomic
publication. A failure before publication removes staging output. Failure to
prove safe device cleanup is a workflow failure even when the samples and
analysis otherwise pass. Evidence must distinguish an analysis rejection from
an acquisition or cleanup failure.

## Hardware-free implementation sequence

Implementation is divided into separately reviewed slices:

1. Implement the Soapy-independent bounded in-memory acquisition component and
   deterministic tests for bounds, reads, non-finite samples, timeout,
   discontinuity, cancellation, and cleanup.
2. The versioned signal-quality analyzer is implemented with synthetic golden
   fixtures; physical-input comparison and qualification remain separate.
3. Implement the live workflow boundary with an injected Soapy API and fake
   devices. This slice is implemented and hardware-free tested for exact
   identity/configuration mapping, pre-device memory validation, acquisition
   and analyzer composition, typed stream and cleanup evidence, cancellation,
   failure classification, and safe cleanup without a physical device.
4. Add an explicit live mode to the production CLI through the shared service.
   This slice is implemented with schema discrimination and no fallback;
   physical execution remains a separate authorization and qualification gate.
5. Add GUI controls over the same service in a separate slice. This slice is
   implemented with explicit schema review, unchanged-request gating, deliberate
   live-run confirmation, injected boundary construction, and hardware-free
   tests; physical execution remains separately gated.
6. Only after all hardware-free gates pass, render a device-, reference-,
   setting-, duration-, abort-, cleanup-, and evidence-bound qualification plan
   for separately authorized execution.

No slice establishes supported-device or calibration claims without retained
qualification evidence for the exact combination.
