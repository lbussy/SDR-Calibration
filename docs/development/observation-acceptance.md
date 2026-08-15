# Observation acceptance

## Scope

Phase 6 implements a deterministic, versioned acceptance policy in the
calibration core. It consumes a carrier-estimator result and explicit
observation, effective-configuration, and reference-suitability diagnostics.
It returns either `accepted`, `rejected`, or `invalid_policy` and retains every
applicable rejection as a stable code plus a reviewable reason.

The evaluator does not acquire or inspect samples, derive the supplied quality
metrics, access SoapySDR or Qt, read a reference registry, fit a calibration
model, calculate uncertainty or reliability scores, serialize profiles, or
override a rejection. Callers remain responsible for deriving the metrics from
reviewable evidence and preserving that evidence with the observation.

## Version 1 policy

The default `observation-acceptance-v1` policy uses inclusive passing
boundaries:

| Diagnostic | Default requirement |
| --- | --- |
| Duration | at least 1 second |
| Signal-to-noise ratio | at least 10 dB |
| Clipped sample fraction | 0 |
| Missing samples | 0 |
| Discontinuities | 0 |
| Absolute fitted drift | at most 1 Hz/s |
| Frequency instability | at most 0.5 Hz |
| Interference-to-carrier ratio | at most -10 dB |
| Effective configuration | verified |
| Reference suitability | suitable, with documented conditions met |

The thresholds are explicit policy inputs so a future reviewed policy can use
different values without changing the evaluator. The version string is copied
into every result. A customized threshold set must use a different, non-empty
version; altered thresholds carrying the built-in `observation-acceptance-v1`
label are invalid. Invalid policy values return `invalid_policy`; they are not
silently replaced by defaults.

Signal-to-noise ratio is the carrier-power to noise-power ratio in decibels.
The clipping fraction is `clipped_samples / estimator_sample_count`.
Frequency instability is the maximum absolute departure, in hertz, from the
accepted fitted carrier model over the observation. Interference-to-carrier is
the strongest interfering component power divided by carrier power in
decibels; a less-negative value is worse. These definitions are input
contracts, not implementations of spectral analysis.

## Fail-closed behavior

Acceptance requires a successful estimator result with finite frequency,
drift, and non-negative reference time plus a positive sample count. Metrics
must be finite, duration must be positive, and
instability and counts must be valid. An estimator failure, inadequate SNR,
clipping, missing samples, discontinuity, excessive drift or instability,
excessive interference, unverified or mismatched effective configuration,
unverifiable or unsuitable reference, or unmet reference conditions rejects
the observation.

The evaluator accumulates all detected failures in deterministic evaluation
order. Consequently, review does not stop at the first defect and downstream
model fitting can enforce the simple rule that only an `accepted` result is
eligible. An unusable or unverifiable observation therefore cannot become a
model input through this interface.

Reference class, evidence authority, provenance, and reliability-quotient
ceilings remain separate from the boolean suitability determination supplied
here. Phase 9 owns the reference registry and evidence implementation; Phase 7
owns model fitting, uncertainty, and versioned assurance scoring.

## Verification boundary

Hardware-free unit tests cover an accepted observation, inclusive threshold
boundaries, simultaneous quality failures, estimator failure, invalid metrics,
invalid policy, configuration states, and reference states. These tests verify
policy behavior only. They do not establish the correctness of externally
derived metrics, reference authority, calibration accuracy, device support, or
qualification.
