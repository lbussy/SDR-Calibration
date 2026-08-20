# Release-candidate readiness

Audit refreshed: 2026-08-20

Prior audited revision: `ea1d4c95e097cb913e189b7f3ed6ed470f336c44`

Artifact-policy update: Decision 0025 replaces the required public-trust MSI
with a required Microsoft Store MSIX. Historical evidence remains identified
by its original artifact and trust state.

## Conclusion

The pre-freeze repository gates are complete. After validation and push, the
clean synchronized commit containing the
[0.1.1 freeze-preparation record](release-candidate-0.1.1-freeze-preparation.md)
is the frozen source candidate. Exact artifact construction, Microsoft Store
certification and delivery, applicable prior-version transitions, final package
binding, and release-ledger gates remain open. Positive physical-device
calibration is not required because this candidate proposes no device-support
claim.

Source designation does not qualify a device, reference, calibration result,
Store delivery/update, package artifact, or distributable release.

## Completed foundations

- Hardware-free capture, estimation, acceptance, model, uncertainty, native
  profile, reference/evidence, shared-workflow, CLI, and GUI components exist
  with deterministic tests.
- The production workflow has passed synthetic end-to-end qualification through
  profile evaluation, including fail-closed integrity cases. This is not
  physical accuracy evidence.
- Airspy HF+ Discovery and SDRplay RSP1B each have retained, exact-combination
  capture evidence. Neither has end-to-end calibration qualification.
- The exact macOS DMG at source `5eb3670` passed signing, notarization,
  stapling, Gatekeeper, payload/license, same-host install, first launch,
  relaunch, CLI help, removal, and restoration checks.
- The exact Windows MSI at source `15ed17b` passed payload/license and
  same-host install, launch, maintenance-install, and uninstall checks using a
  locally trusted self-signed development certificate.
- Historical Store MSIX candidates have passed WACK and same-host development
  lifecycle checks. Those results are artifact-specific and do not transfer to
  a newly frozen package; the current candidate must record its own hash and
  disposition in RC-05.
- An offline [Store submission-readiness packet](windows-store-submission-readiness.md)
  now provides proposed listing copy, `runFullTrust` justification, preliminary
  privacy and age-rating assessments, four exact-candidate screenshots, a
  deterministic certification fixture attachment, and a fail-closed preflight.
  Read-only Partner Center inspection is retained; owner choices, upload,
  certification, and publication remain open.
- Raspberry Pi OS 13 ARM64 has native CLI build, package, and measured resource
  evidence for the recorded-input path.
- The project owner confirmed clean-host lifecycle passes for the exact indexed
  macOS DMG, self-signed Windows MSI, and Raspberry Pi DEB hashes; see the
  [owner-confirmed record](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/clean-host/2026-08-17-owner-confirmed/README.md).
- The public repository has GitHub Private Vulnerability Reporting enabled and
  a finalized supported-version/security policy with retained maintainer-side
  channel-control evidence. Candidate-specific access revalidation remains part
  of final release assembly.

## Blocking ledger

| ID | Gate | Current state | Dependency or unblock condition | Required retained evidence |
| --- | --- | --- | --- | --- |
| RC-01 | Optional matched receiver comparison | Deferred; separate captures do not control shared propagation and feed conditions | Required only before receiver-equivalence, cross-device, interchangeability, or general receiver-support claims; not a blocker for user-directed calibration | Exact identities/configurations, common observation windows, port rotation, results, abort, and cleanup |
| RC-02 | Observation-specific reference suitability | The exact five-second 10 MHz Airspy observation is accepted within its `ad_hoc` limits. Both 20 MHz attempts remain rejected by the unchanged policy; the filtered diagnostic nevertheless proves reception and estimation behavior | Reopen only for a proposed accepted observation or stronger reference claim. A second accepted frequency is required for an optional positive linear-model physical cell, not for release of the user-directed workflow | Exact reference identity, conditions, uncertainty, assurance ceiling, observation decision, hashes, cleanup, and safe-state evidence |
| RC-03 | Optional positive physical end-to-end cells | No positive cell; not required for the initial user-directed workflow claim | Required only for a proposed positive physical-device cell. The mandatory release gate instead binds the production candidate to the composed physical and deterministic workflow evidence under decision 0028 | Exact cell evidence from accepted observations through evaluation, or an explicit `Not qualified` disposition; no transfer to other devices or conditions |
| RC-WF | User-directed workflow validation | Passed for exact source candidate `b269f9c`; retained physical artifacts were hash-reverified and 21 baseline plus 13 focused deterministic tests passed. Final packaged-candidate binding remains open | Freeze the package candidate and prove its exact artifacts derive from the validated source and preserve the composed evidence chain and fail-closed claim boundaries | [Source-candidate binding record](rc-wf-source-candidate-binding.md), followed by exact package hashes, lifecycle results, privacy review, and release-ledger linkage |
| RC-04 | macOS clean-host lifecycle | Passed by owner confirmation for exact DMG SHA-256 `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4` | Complete | Exact hash and owner-confirmed lifecycle record retained |
| RC-05 | Windows Store certification and lifecycle | Current frozen-candidate construction and package audit are required; historical WACK, development lifecycle, screenshots, fixtures, upload, and certification evidence remain historical and do not transfer | Run WACK and the development lifecycle for the exact frozen MSIX where separately authorized; Store submission, certification, and delivery remain later gates | Exact candidate hash and identity/version; candidate-specific WACK and lifecycle disposition; later certification result, Microsoft signature, Store install, removal, cleanup, and listing/capability agreement |
| RC-06 | Prior-version transitions and Windows MSI migration | Exact read-only `0.1.0` macOS/DEB baselines exist; no transition is qualified. No published Store predecessor exists for the first MSIX | Freeze a candidate above `0.1.0`; execute applicable macOS/DEB transitions; independently verify the first-Store-release N/A; qualify MSI coexistence/manual migration | Old/new hashes, version consistency, preserved seeded state, package-manager transitions, Windows coexistence/CLI/Start identity, cleanup, and independent review |
| RC-07 | Bind accepted compatibility and artifact policy to a candidate | Decisions 0022, 0025, and 0028 define the policy; the [0.1.1 freeze-preparation record](release-candidate-0.1.1-freeze-preparation.md) binds the intended schema, artifacts, construction paths, and invalidation rule without naming an unknown future revision | Complete pre-freeze validation, commit and push the preparation, then designate that exact clean synchronized revision before constructing new artifacts | Frozen revision, exact schemas and artifacts, compatibility statement, construction evidence, and no-post-freeze-change rule |
| RC-08 | Release documentation and evidence ledger | Fail-closed templates and evidence-package assembly plan exist; no frozen-candidate values or final approvals are populated | RC-07 plus exact final candidate artifacts; every required cell must be linked, digested, privacy-reviewed, and independently verified | Completed release notes, known limitations, upgrade/rollback guidance, vulnerability reporting, checksums, manifest, inventories, corresponding source, and evidence index with no blocking required fields |

RC-WF is the release-enabling calibration-workflow gate. Physical acquisition,
truthful rejection, and deterministic later-stage evidence complement rather
than substitute for one another. RC-03 remains the optional accuracy-bearing
physical-cell matrix; passing one cell does not close another or create a
general support claim. RC-04 is complete. RC-05 remains
open until the exact frozen MSIX is certified and Store-delivered. RC-06 cannot
be satisfied until candidate `0.1.1` completes its applicable transitions.

A Store-MSIX source packaging contract now implements the required Windows
release route. The `SDR Calibration` product name and exact
non-secret manifest identity are reserved in an active Individual developer
account. Native build and local lifecycle evidence exists, but no package has
been uploaded, no certification has occurred, and no Microsoft-provided
signature has been observed. Decision 0025 makes this the primary Windows
artifact; the self-signed MSI is optional testing evidence only.

## Resolved scope decisions

Decisions [0022](decisions/0022-initial-compatibility-and-release-artifacts.md)
and [0025](decisions/0025-store-msix-primary-windows-artifact.md) resolve the
initial artifact and compatibility scope as follows:

- macOS ARM64 DMG, Microsoft Store Windows x64 MSIX, and Raspberry Pi OS 13 ARM64
  CLI DEB are intended binary artifacts, accompanied by source, checksums,
  notices, inventories, corresponding-source material, and release evidence.
- Initial binary packages are recorded-input-only. They do not bundle SoapySDR,
  vendor modules, capture tooling, or live-device support.
- The native profile schema-major-1 semantics are the durable compatibility
  promise. Strict run requests, run-bound evidence, and pre-1.0 package/UI
  behavior have narrower exact-version promises.
- Unsigned profiles remain permitted only with explicit signature state and
  applicable assurance consequences. Production signing is not implied.
- Profile activation/evaluation orchestration and installed WSJT-X mutation are
  outside the initial artifact scope.
- No device becomes supported merely because a source-built live path or
  capture record exists; exact positive claims still require retained evidence.

## Hardware-free work that may proceed

Version identity is centralized on the CMake project version and guarded by
source-contract and runtime tests. Version `0.1.1` is selected for candidate
preparation, but it is not frozen by an exact synchronized source revision and
artifact hash and is not a release-candidate designation.

Upgrade/rollback procedures and acceptance criteria are prepared in the
[qualification plan](upgrade-rollback-qualification-plan.md). Execution remains
blocked until a higher-version frozen candidate exists.

The release-documentation templates are retained in the
[release template pack](release/index.md). Their existence does not complete
RC-08; every `TBD-BLOCKING` value must be bound to the exact frozen candidate
and reviewed before release authorization.

The [evidence package assembly plan](release/evidence-package-assembly-plan.md)
is prepared. It defines the final public/private boundary, directory layout,
manifest, checksums, evidence cells, validation, and independent review without
claiming that a candidate package has been assembled.

## Ordered resumption path

1. Bind the frozen package artifacts to RC-WF's passing source-candidate record
   and verify that they preserve the complete user-directed workflow and claim
   boundary; no additional hardware test is required.
2. Retain RC-01 as optional until a receiver-equivalence, cross-device,
   interchangeability, or general receiver-support claim is proposed.
3. Reopen RC-02 and run RC-03 only for a proposed accepted observation or
   positive physical-device cell; neither blocks the initial workflow claim.
4. Complete Windows Store certification/delivery and RC-06 applicable
   prior-version and MSI-migration qualification for the frozen candidate.
5. Complete RC-07, build only the frozen candidate artifacts, and assemble
   RC-08 from their exact hashes and retained evidence.
6. Perform a final adversarial release-candidate audit before any tag,
   publication, or release action.
