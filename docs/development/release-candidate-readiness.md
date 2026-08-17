# Release-candidate readiness

Audit date: 2026-08-17

Audited revision: `ea1d4c95e097cb913e189b7f3ed6ed470f336c44`

Artifact-policy update: Decision 0025 replaces the required public-trust MSI
with a required Microsoft Store MSIX. Historical evidence remains identified
by its original artifact and trust state.

## Conclusion

The project is not ready to freeze an initial release candidate. Its
hardware-free application foundation, package evidence, and owner-confirmed
clean-host lifecycle results are substantial, but physical calibration,
Microsoft Store certification and delivery, applicable prior-version upgrade,
candidate binding, and release-ledger gates remain open.

This is a readiness audit, not a release-candidate designation. It does not
qualify a device, reference, calibration result, Store delivery/update, or
distributable release.

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
- The exact unsigned Store MSIX at source `99d96a6` passed native construction,
  payload/license audit, and a same-host development-signed install, CLI alias,
  GUI launch/relaunch, uninstall, and cleanup lifecycle. It has not been
  uploaded, certified, Store-signed, or Store-delivered.
- An offline [Store submission-readiness packet](windows-store-submission-readiness.md)
  now provides proposed listing copy, `runFullTrust` justification, preliminary
  privacy and age-rating assessments, an asset plan, and a fail-closed
  preflight. Partner Center inspection, owner choices, exact-candidate assets,
  upload, certification, and publication remain open.
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
| RC-01 | Matched receiver comparison | Blocked; separate captures do not control shared propagation and feed conditions | Install the splitter and approve a bounded simultaneous Airspy/RSP1B plan | Exact identities/configurations, common observation windows, port rotation, results, abort, and cleanup |
| RC-02 | Reference suitability | Blocked; the readiness package intentionally establishes no physical suitability | Install the splitter, complete source/path dossiers, and approve the survey plan | Reference identity/authority, topology, levels, attenuation, conditions, uncertainty, acceptance, and cleanup |
| RC-03 | Physical end-to-end calibration | Not qualified | RC-01 and RC-02 must supply an accepted device/path/reference combination | Acquisition through profile evaluation, comparison error/uncertainty, assurance ceiling, exact artifacts, and cleanup |
| RC-04 | macOS clean-host lifecycle | Passed by owner confirmation for exact DMG SHA-256 `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4` | Complete | Exact hash and owner-confirmed lifecycle record retained |
| RC-05 | Windows Store certification and lifecycle | Local MSIX construction and development-signed same-host lifecycle passed for SHA-256 `de28c7d9c11c1fcf95e70ffc80a5147c6dfd5f26e93c8f6138906a6f6ba95f85`; Store certification/delivery and clean-host evidence remain open | Freeze and submit the exact candidate under separate authorization, pass certification, then qualify Store installation on a clean Windows 11 host | Exact identity/version; certification result; Microsoft signature; Store install, GUI, CLI alias, removal, cleanup, listing/capability agreement, and retained evidence |
| RC-06 | Prior-version transitions and Windows MSI migration | Exact read-only `0.1.0` macOS/DEB baselines exist; no transition is qualified. No published Store predecessor exists for the first MSIX | Freeze a candidate above `0.1.0`; execute applicable macOS/DEB transitions; independently verify the first-Store-release N/A; qualify MSI coexistence/manual migration | Old/new hashes, version consistency, preserved seeded state, package-manager transitions, Windows coexistence/CLI/Start identity, cleanup, and independent review |
| RC-07 | Bind accepted compatibility and artifact policy to a candidate | Decisions 0022 and 0025 define the policy; no source or schema revision is frozen | Select a candidate only after its required artifact and qualification gates can be executed | Frozen revision, exact schemas and artifacts, compatibility statement, and change-control rule |
| RC-08 | Release documentation and evidence ledger | Fail-closed templates and evidence-package assembly plan exist; no frozen-candidate values or final approvals are populated | RC-07 plus exact final candidate artifacts; every required cell must be linked, digested, privacy-reviewed, and independently verified | Completed release notes, known limitations, upgrade/rollback guidance, vulnerability reporting, checksums, manifest, inventories, corresponding source, and evidence index with no blocking required fields |

RC-03 is the accuracy-bearing gate. Package success, synthetic workflow success,
or capture success cannot substitute for it. RC-04 is complete. RC-05 remains
open until the exact frozen MSIX is certified and Store-delivered. RC-06 cannot
be satisfied until a candidate newer than `0.1.0` is frozen and tested.

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

Version identity is now centralized on the CMake project version and guarded by
source-contract and runtime tests. A candidate version is not selected or
frozen by that implementation.

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

1. Complete RC-01 after the splitter is installed.
2. Complete RC-02 with the controlled reference path.
3. Run RC-03 for each exact device/reference/platform combination proposed for
   a positive support claim.
4. Complete Windows Store certification/delivery and RC-06 applicable
   prior-version and MSI-migration qualification for the frozen candidate.
5. Complete RC-07, build only the frozen candidate artifacts, and assemble
   RC-08 from their exact hashes and retained evidence.
6. Perform a final adversarial release-candidate audit before any tag,
   publication, or release action.
