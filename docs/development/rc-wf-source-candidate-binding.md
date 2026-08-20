# RC-WF source-candidate binding

## Disposition

**Passed for subject source candidate
`b269f9ccb26a44f2720c578194aa9b3074b7223d`.**

This record binds one exact source candidate to the composed evidence required
by [decision 0028](decisions/0028-user-directed-calibration-workflow.md). It
validates the user-directed calibration and evidence-qualification workflow. It
does not designate a final release candidate, qualify a positive physical-device
cell, or establish general hardware support.

The subject was clean and synchronized with `origin/main` before validation.
The local toolchain was CMake 4.4.2, Apple clang 21.0.0, Qt 6.11.1, SoapySDR
library 0.8.1-release, API 0.8.0, and ABI 0.8.

## Contract versions

The subject binds schema major 1 and the production contracts
`frequency-error-model-v1`, `observation-acceptance-v1`,
`signal-quality-v1`, `uncertainty-budget-v1`, and
`reliability-quotient-v1`. Measurement uncertainty and the reliability quotient
remain separate: the former quantifies the measurement result, while the latter
indexes evidence assurance and remains subject to the reference-class ceiling.

## Evidence binding

| Workflow stage | Evidence class | Subject-candidate disposition |
| --- | --- | --- |
| Bounded physical acquisition and requested/effective settings | Historical physical | Retained exact Airspy and SDRplay capture records establish bounded transport, setting evidence, stream statistics, and cleanup only for their recorded combinations. Synthetic tests do not replace this evidence. |
| Physical carrier estimation | Historical physical | The accepted 10 MHz and both 20 MHz records retain production estimator results. They do not create a general device claim. |
| Observation acceptance and truthful rejection | Historical physical plus candidate deterministic | The 10 MHz observation retains its bounded accepted disposition. Both 20 MHz observations remain rejected by the unchanged instability policy. Candidate tests prove rejected observations stop before fitting. |
| Bounded acquisition, cancellation, and cleanup behavior | Candidate deterministic | `capture_unit_tests`, `memory_acquisition_unit_tests`, and `soapy_adapter_unit_tests` passed, including bounds, partial/failure handling, cancellation, reverse-order cleanup, and known-safe-state enforcement through fake APIs. |
| Estimator and analyzer behavior | Candidate deterministic | `carrier_estimator_unit_tests` and `signal_quality_analyzer_unit_tests` passed their synthetic golden and rejection fixtures. |
| Linear fitting and uncertainty | Candidate deterministic | `model_uncertainty_unit_tests` and `application_workflow_unit_tests` passed finite fitting, residual, validity-domain, correlation, coverage-factor, and material-component cases. |
| Reliability quotient and reference ceiling | Candidate deterministic | Model, workflow, and reference/evidence tests passed the versioned component calculation, reduced-assurance, and reference-ceiling cases without treating the quotient as uncertainty or probability. |
| Native profile generation, validation, and evaluation | Candidate deterministic | `profile_engine_unit_tests` and `application_workflow_unit_tests` passed canonicalization, semantic validation, integrity, in-domain evaluation, sign convention, tamper rejection, and extrapolation rejection. |
| Registry, trust, provenance, and privacy | Candidate deterministic | `reference_evidence_unit_tests`, workflow tests, and production CLI tests passed authenticated-registry, independent trust-pin, evidence-linkage, privacy, invalid-input, and reduced-assurance cases. |
| Atomic publication and failure cleanup | Candidate deterministic | `production_cli_tests` passed new-directory publication, existing-output refusal, staging cleanup, finalized evidence binding, and no completed partial result on failure. |
| CLI and GUI alignment | Candidate deterministic | `production_cli_tests`, `gui_workflow_tests`, and `gui_widget_tests` passed the shared production-service, schema-review, cancellation, progress, and artifact-review contracts. |

## Retained physical artifact verification

The three relevant private captures and manifests were present during this
review. Their sizes and SHA-256 values matched the already committed public
records:

| Observation | Capture bytes | Capture SHA-256 | Manifest bytes | Manifest SHA-256 |
| --- | ---: | --- | ---: | --- |
| Accepted 10 MHz | 7,680,000 | `447091e5ade3f982f3f68656faee42d48fa1ae41bc30ba89ccae4f302e56f53f` | 2,977 | `39c567b2746b21d0ba10e994b0f40bef0b94b1727fc17de80deaaf2d5a1122ab` |
| Rejected unfiltered 20 MHz | 7,680,000 | `38408580ec22971ee27ffac0ae6aea1ea58a669ee311e41aa6d516fa4a2d58ee` | 2,983 | `fbd8bcdda5e0052da0ec09bda6e8a81cf04d0da305675c283626535099c66d69` |
| Rejected filtered 20 MHz diagnostic | 7,680,000 | `f6ec6847299782208ea9a36fa231fd31f7d296b042c5b5682e0f7555c9822909` | 2,967 | `80c993a4aeaf9ffa70a06ce726052fb19325157da819a9d3eb8a84b61b429e1a` |

The files were read only for size and digest verification. They were not
replayed: the subject candidate does not expose a production raw-IQ path that
derives the full calibration workflow from these captures, and a temporary
custom harness would not constitute production candidate binding. Their
previous analysis remains historical physical evidence. No private path,
device serial, reference-source serial, or location is published here.

## Validation

The following hardware-free commands passed:

```text
cmake --preset default
cmake --build --preset default
ctest --preset default
```

The baseline contained 21 tests and all 21 passed. A focused second run of the
13 capture, memory acquisition, estimator, analyzer, acceptance, model and
uncertainty, profile, reference/evidence, application workflow, production CLI,
GUI workflow, GUI widget, and fake-Soapy suites also passed.

Sphinx with warnings as errors and `git diff --check` are run against the final
evidence-record change before integration.

## Claim boundary and remaining binding

The source candidate passes RC-WF because the composed evidence demonstrates
real bounded acquisition and real acceptance behavior while the exact subject
implementation deterministically exercises every later production stage. The
accepted and rejected observations retain their original dispositions; no
threshold, reference authority, uncertainty input, assurance score, or profile
claim was changed.

Final packaged-candidate binding remains open. After candidate freeze, the exact
macOS, Windows, and Raspberry Pi artifacts must be linked to the subject source,
schemas, deterministic results, package lifecycle evidence, checksums, privacy
review, and release ledger. RC-01, a second accepted Airspy frequency, and a
positive RC-03 cell remain optional unless a receiver-equivalence, cross-device,
or positive physical-device claim is proposed.

No hardware was enumerated, opened, configured, or streamed during this work.

## Adversarial assessment

The final review checked for historical evidence presented as candidate output,
rejected input admitted to fitting, synthetic evidence presented as physical,
physical capture presented as later-stage proof, uncertainty/assurance
conflation, unsupported certification or traceability language, identity or
version mismatch, private-data leakage, extrapolation, partial-publication
overclaim, and accidental RC-01/RC-02/RC-03 release blocking.

One actionable finding was closed: the top-level overview and proposed Store
short description unconditionally called the generated profiles `traceable`.
Both now say `evidence-bounded`, and the overview permits `traceable` only when
the specific result satisfies the reference and provenance contract. Because
the prior Store text was already saved externally, the Store readiness record
now requires Partner Center reconciliation before final listing agreement. No
external state was changed during this correction.

No remaining actionable finding was identified within RC-WF scope.
