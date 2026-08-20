# 0028 — User-directed calibration workflow validation

Status: Accepted

Date: 2026-08-20

## Decision

The remaining release-enabling hardware objective is to demonstrate and
validate the complete user-directed calibration and evidence-qualification
workflow. It is not to certify the project owner's SDR, reference source,
filter, or RF path as supported hardware.

The phrase *user certification* means that the application lets a user collect
and review evidence for the user's own exact device and conditions. It does not
mean regulatory certification, accredited metrological certification, vendor
endorsement, general device support, or traceability beyond the evidence. The
claim levels in [decision 0005](0005-scope-and-claim-levels.md) remain
authoritative.

Release-enabling workflow evidence may be composed from independently retained
fixtures when every boundary is explicit:

- bounded physical acquisition establishes that requested and effective
  settings, samples, stream state, cleanup, and provenance can be collected;
- accepted and rejected physical observations establish that production
  estimation and suitability policy behave truthfully on real inputs;
- deterministic synthetic or reviewed recorded observations exercise fitting,
  uncertainty, assurance, native-profile generation, validation, evaluation,
  integrity, privacy, and evidence publication;
- the final candidate binds the same production services and contracts used by
  those fixtures.

A rejected physical observation is positive workflow-validation evidence when
the software correctly derives and reports the rejection. It is never an
accepted model input. A successful multi-frequency physical profile may provide
additional evidence for its exact cell, but it is not required to certify the
project owner's equipment or to enable users to calibrate their own equipment.

RC-01 and the device matrix remain optional gates for receiver equivalence,
cross-device interchangeability, or general supported-hardware claims. RC-02
remains an observation-specific reference-suitability contract. RC-03 remains
the optional matrix for positive physical end-to-end device cells defined by
[decision 0027](0027-cell-specific-physical-calibration.md). None is a release
blocker unless the proposed release makes the corresponding positive claim.

## Consequences

- The accepted 10 MHz Airspy observation, rejected unfiltered 20 MHz attempt,
  and rejected filtered 20 MHz diagnostic remain bounded historical evidence.
  Together they demonstrate real acquisition, estimation, acceptance, and
  rejection behavior; they do not establish general Airspy, filter, or
  reference-path support.
- The initial release may enable evidence-bounded user calibration without a
  positive project-owner RC-03 cell or completed RC-01 comparison.
- Release review must prove that the production candidate preserves the
  validated workflow composition and clearly communicates the limits of each
  resulting observation, profile, and claim.
- Observation thresholds, identity, integrity, reference, uncertainty,
  provenance, assurance, privacy, and cleanup rules remain fail closed.
- A future supported-device or cross-device claim reopens the applicable
  RC-01/RC-02/RC-03 evidence gates without changing this release boundary.
