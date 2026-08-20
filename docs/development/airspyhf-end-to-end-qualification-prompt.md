# Airspy HF+ end-to-end qualification execution prompt

Historical optional-device-cell prompt. Decision 0028 removed this exact
project-owner hardware cell from the mandatory release path. It remains a
bounded plan only for a future separately authorized positive Airspy cell and
does not authorize hardware access.

## Objective

Qualify, or fail closed without qualification, one exact local end-to-end live
calibration combination using the Airspy HF+ Discovery and NIST WWV carrier
references. Retain independently reviewable evidence for acquisition through
publication and cleanup without generalizing the result to other combinations.

## Exact candidate

- Source: the clean immutable revision selected immediately before execution.
- Host: the local Apple Silicon Mac, with its OS build recorded at execution.
- SoapySDR: exact library, API, ABI, and SoapyAirspyHF module versions recorded
  at execution.
- Device: Airspy HF+ Discovery, serial `2f52ff5de72635ba`, driver argument
  `airspyhf`, receive channel 0, RX port, operator's existing receive antenna.
- References: NIST WWV nominal carriers at 10,000,000 Hz and 15,000,000 Hz.
  Both are required because the production workflow fits a linear model from at
  least two independently identified observations with nonzero frequency span.
- Receive settings: 192,000 complex samples/s, no requested hardware bandwidth,
  no requested aggregate gain, strict requested/effective policy.
- Bounds: one five-second acquisition per observation, 100 ms read timeout, and
  a 16 MiB in-memory ceiling. No raw-IQ publication by the production workflow.
- Output: a new timestamped directory under a private temporary qualification
  root, promoted to repository evidence only after privacy and completeness
  review.
- Abort: cooperative SIGINT/cancel first; if cleanup cannot establish a known-
  safe state, stop the gate and inspect the process/device without retrying.

## Authorization boundary

Rendering, source inspection, retained-evidence inspection, request generation,
builds, fake tests, and documentation checks are hardware-free. The following
operations require separate explicit authorization naming this exact device and
bounds:

1. receive-only enumeration of Airspy serial `2f52ff5de72635ba`;
2. at most one five-second diagnostic capture at each of 10 MHz and 15 MHz; and
3. only if readiness and both diagnostics pass, one production live run with
   two five-second observations and the same limits.

No transmission, network-controlled receiver, service installation, reboot,
driver change, package installation, or unbounded retry is authorized.

## Stage A: hardware-free readiness gate

1. Verify a clean synchronized worktree and freeze the source revision.
2. Verify the production request can represent the exact expected device
   identity and all configuration fields required by `SoapyWorkflowBoundary`.
3. Compare those requirements with the retained Qualification Gate 1 manifest.
4. Verify current NIST primary sources establish the nominal carrier frequencies
   and document the distinction between transmitted accuracy and received HF
   uncertainty.
5. Stop before enumeration if required identity/configuration readback is absent
   or if exact hardware authorization has not been granted.

## Stage B: bounded device readiness

After explicit authorization, enumerate only the serial-bound device and record
the exact resolved arguments and versions. Run the two bounded diagnostic
captures without changing thresholds. Each must have exact sample count, zero
missing samples and discontinuities, known-safe cleanup, and a carrier suitable
for the production estimator/analyzer. A failed diagnostic makes the gate
`Not qualified`; do not tune acceptance policy to force a pass.

## Stage C: production execution

Create a strict `sdrcal-live-calibration-request` with two independent WWV
observations, exact requested/effective identity and configuration, an
authenticated locally pinned registry snapshot, explicit propagation and
received-signal uncertainty, honest assurance components, and bounded evidence.
Execute through the production CLI or GUI once. Preserve canonical terminal
JSON, diagnostics, timestamps, configuration and stream evidence, analyzer
metrics, acceptance reasons, model and uncertainty results, profile/evidence
artifacts, hashes, cleanup, and post-run device availability.

## Acceptance and review

Qualification requires every item in the end-to-end qualification contract,
including authoritative-reference traceability, propagation limitations,
accepted observations, valid nonzero-span linear fit, uncertainty, in-domain
profile evaluation against the reference, atomic publication, independent
hashes, and known-safe cleanup. Any missing, unverified, conflicting, or
privacy-sensitive evidence fails closed.

Adversarially review identity substitution, configuration assumptions,
WWV/WWVH ambiguity, propagation uncertainty, metric provenance, observation
independence, threshold tuning, partial publication, cleanup, and claim scope.
Correct documentation defects, but do not repair a failed physical result by
changing scientific or safety policy.

## Exit criteria

The outcome is either a narrowly positive matrix entry for this exact retained
combination or a documented `Not qualified` result with the first blocking
gate. No broader device, platform, package, reference, or calibration claim is
created.
