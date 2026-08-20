# Carrier estimator

## Scope

Phase 5 implements a deterministic carrier estimator in the calibration core.
It consumes an in-memory sequence of uniformly spaced complex samples and the
effective sample rate. It does not read capture artifacts, access an SDR, use
SoapySDR or Qt, inspect operating-system paths, serialize profiles, decide
whether an observation is acceptable, or calculate measurement uncertainty.

The estimator reports the carrier offset at the temporal midpoint of the input
and a linear drift rate. A later application service is responsible for
combining the offset with the effective tuned frequency and reference metadata.
The separate Phase 6 observation-acceptance component owns observation-quality
policy.

## Input contract

- Samples are ordered, uniformly spaced complex values.
- The sample rate is finite, positive, and is the effective rate associated
  with the samples.
- At least 64 samples are required by the default policy.
- Every real and imaginary component must be finite.
- The default estimator range is limited to `+-0.45 * sample_rate_sps` to
  retain a margin from phase-increment ambiguity at Nyquist.

Callers may set estimator options explicitly, but invalid option combinations
fail with `invalid_options`; excessive iteration counts are rejected. Options
are processing controls, not observation-acceptance or
qualification policy.

## Method and outputs

The implementation forms adjacent-sample conjugate products, fits their phase
increments to a linear-in-time frequency model, and applies deterministic
Huber reweighting. The fit returns:

- carrier frequency offset in hertz at `reference_time_seconds`;
- linear drift in hertz per second;
- input mean power;
- residual model coherence and phase RMS diagnostics; and
- a status and reviewable reason.

The coherence and phase-RMS diagnostics summarize residuals of the
**adjacent-sample phase increments** used by the frequency fit. They are not a
whole-observation coherent-carrier-power or SNR calculation. Small
time-varying frequency departures can remain highly coherent from one sample
to the next while their integrated phase accumulates across a longer
observation. The separate signal-quality analyzer removes the fitted phase over
the complete buffer and can therefore report low SNR despite estimator
coherence near one.

This estimator makes no uncertainty, confidence, accuracy, reference-
suitability, device-support, or calibration claim. Its coherence floor rejects
inputs whose adjacent phase increments cannot support a single drifting-carrier
estimate; passing that floor does not establish long-duration phase coherence
or make an observation acceptable. Clipping, SNR, discontinuity,
instability, and interference are evaluated by the separate observation-
acceptance policy even though estimator fixtures exercise representative
conditions.

## Failure behavior

The estimator rejects invalid sample rates, short inputs, non-finite samples,
insufficient energy, incoherent or singular phase evolution, and a model whose
frequency crosses the configured Nyquist margin anywhere in the observation. A
failed result leaves the optional frequency, drift, and reference-time fields
disengaged so downstream code cannot mistake a default zero for an estimate.

## Verification boundary

Golden unit fixtures cover known positive and negative frequency offsets,
linear drift, seeded noise, component clipping, a competing carrier, silence,
short input, non-finite samples, invalid sample rate, and an out-of-range tone.
They are synthetic, hardware-free estimator tests. They do not qualify an SDR,
reference source, observation, calibration model, uncertainty budget, profile,
or end-to-end calibration result.
