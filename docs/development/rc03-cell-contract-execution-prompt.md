# RC-03 cell-contract reconciliation execution prompt

Historical execution prompt. Its release-gating objective was superseded by
[decision 0028](decisions/0028-user-directed-calibration-workflow.md). The
cell-specific evidence rules remain applicable only when a positive physical
device-cell claim is proposed.

## Objective

Resolve the release-wide RC-01 dependency without bypassing it: define physical
end-to-end calibration as exact qualification cells, retain RC-01 for every
cross-device claim, and identify the maximum honest Airspy-only progress under
the accepted five-second LBE-1421 reference boundary.

## Scope

- Record the cell-specific decision and update roadmap, status, and development
  navigation consistently.
- Retain the production linear-model requirement and document the additional
  nonzero-frequency-span evidence needed for an Airspy-only attempt.
- Preserve the authenticated-registry, reference-condition, uncertainty,
  assurance-ceiling, native-profile, evaluation, evidence, privacy, and cleanup
  gates.
- Keep RC-01 blocked pending the splitter and RC-03 `Not qualified` pending a
  completed physical cell.

## Non-goals and safety boundary

- Do not enumerate, open, configure, connect, disconnect, power, or stream from
  an SDR or reference source.
- Do not alter calibration algorithms, schemas, thresholds, trust verification,
  profile semantics, or application behavior.
- Do not create or activate a registry, signature, trust pin, run request,
  profile, or qualification artifact.
- Do not claim traceability, receiver equivalence, RSP1B support, cross-device
  support, arbitrary-duration suitability, RC-01 completion, or RC-03
  completion.

## Required disposition

The exact Airspy HF+ Discovery cell may proceed independently of RC-01 only
after all other contracts are satisfied. Its current production path is
recorded ingestion through `sdrcal calibrate`, using at least two independently
accepted observations with nonzero indicated-frequency span. The accepted 10
MHz five-second artifact can be only one observation after private-artifact and
condition-evidence reverification. At least one different reference frequency
must receive a new RC-02 qualification and digest-bound hardware authorization.

An authenticated, unexpired `ad_hoc` registry must bind the exact private
LBE-1421 identity, nominal frequencies, 0.5 Hz standard reference uncertainty,
ceiling 50, evidence, conditions, limitations, status, and expiry. The
independent trust file must authenticate it. At ceiling 50 the native result is
informational-only; WSJT-X export and every qualification-capable claim are
forbidden.

## Validation and review

Run the hardware-free CMake configure, build, and CTest baseline; build Sphinx
with warnings as errors; and run `git diff --check`. Review for RC-01 bypass,
fabricated independence or frequency span, trust weakening, caller-asserted
conditions presented as proof, private-data leakage, profile overclaim,
extrapolation, and inconsistent roadmap language. Correct every actionable
finding and repeat affected checks.

## Exit criteria

The durable contract permits an exact Airspy-only RC-03 cell without weakening
RC-01, identifies a different-frequency Airspy observation rather than a
different SDR as the next physical need, leaves all hardware work separately
authorized, and keeps RC-03 `Not qualified` until complete physical evidence
exists.
