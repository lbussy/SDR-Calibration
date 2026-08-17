# Release-candidate readiness

Audit date: 2026-08-17

Audited revision: `ea1d4c95e097cb913e189b7f3ed6ed470f336c44`

## Conclusion

The project is not ready to freeze an initial release candidate. Its
hardware-free application foundation and exact macOS and Windows same-host
package evidence are substantial, but physical calibration, fresh-host
installation, compatibility policy, and release-ledger gates remain open.

This is a readiness audit, not a release-candidate designation. It does not
qualify a device, reference, calibration result, clean installation, upgrade,
public Windows signature, or distributable release.

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
- Raspberry Pi OS 13 ARM64 has native CLI build, package, and measured resource
  evidence for the recorded-input path.

## Blocking ledger

| ID | Gate | Current state | Dependency or unblock condition | Required retained evidence |
| --- | --- | --- | --- | --- |
| RC-01 | Matched receiver comparison | Blocked; separate captures do not control shared propagation and feed conditions | Install the splitter and approve a bounded simultaneous Airspy/RSP1B plan | Exact identities/configurations, common observation windows, port rotation, results, abort, and cleanup |
| RC-02 | Reference suitability | Blocked; the readiness package intentionally establishes no physical suitability | Install the splitter, complete source/path dossiers, and approve the survey plan | Reference identity/authority, topology, levels, attenuation, conditions, uncertainty, acceptance, and cleanup |
| RC-03 | Physical end-to-end calibration | Not qualified | RC-01 and RC-02 must supply an accepted device/path/reference combination | Acquisition through profile evaluation, comparison error/uncertainty, assurance ceiling, exact artifacts, and cleanup |
| RC-04 | macOS clean-host lifecycle | Same-host lifecycle only | A genuinely fresh supported macOS ARM64 host or equivalent approved clean image | Exact DMG hash; clean-user install, first launch, normal launch, CLI, removal, and residue review |
| RC-05 | Windows clean-host and public-trust lifecycle | Same-host self-signed lifecycle only; decision 0022 requires a public-trust MSI | Fresh Windows 11 x64 host and an accepted public-trust signing arrangement | Exact MSI hash; trust mode, timestamp, SmartScreen observations, lifecycle, and cleanup |
| RC-06 | Prior-version upgrade and rollback | Exact read-only, content-addressed `0.1.0` macOS, Windows, and Raspberry Pi baselines are retained; no upgrade or rollback is qualified | Freeze a candidate above `0.1.0` and provide suitable fresh-host test arrangements | Old/new hashes, preserved user state, migration outcome, rollback procedure, and residue review |
| RC-07 | Bind accepted compatibility and artifact policy to a candidate | Decision 0022 defines the policy; no source or schema revision is frozen | Select a candidate only after its required artifact and qualification gates can be executed | Frozen revision, exact schemas and artifacts, compatibility statement, and change-control rule |
| RC-08 | Release documentation and evidence ledger | Fail-closed templates exist; no frozen-candidate values or final approvals are populated | RC-07 plus exact final candidate artifacts; physical and clean-host results must be linked, not inferred | Completed release notes, known limitations, upgrade/rollback guidance, vulnerability reporting, checksums, inventories, and evidence index with no `TBD-BLOCKING` fields |

RC-03 is the accuracy-bearing gate. Package success, synthetic workflow success,
or capture success cannot substitute for it. RC-04 through RC-06 are
distribution lifecycle gates and cannot be satisfied by another same-host run.

## Resolved scope decisions

Decision [0022](decisions/0022-initial-compatibility-and-release-artifacts.md)
resolved the initial artifact and compatibility scope as follows:

- macOS ARM64 DMG, public-trust Windows x64 MSI, and Raspberry Pi OS 13 ARM64
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

1. Design clean-host macOS and Windows lifecycle plans bound to exact candidate
   artifacts, but do not simulate a clean host on the development machine.
2. Finalize platform-specific upgrade/rollback acceptance criteria against the
   [retained 0.1.0 baselines](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/release-baselines/0.1.0/README.md).
3. Design the required Raspberry Pi clean-host lifecycle plan.
4. Audit final icon rendering on Windows and any other platform included in the
   release artifact set; the current retained human review covers macOS only.

The release-documentation templates are retained in the
[release template pack](release/index.md). Their existence does not complete
RC-08; every `TBD-BLOCKING` value must be bound to the exact frozen candidate
and reviewed before release authorization.

## Ordered resumption path

1. Complete RC-01 after the splitter is installed.
2. Complete RC-02 with the controlled reference path.
3. Run RC-03 for each exact device/reference/platform combination proposed for
   a positive support claim.
4. Execute RC-04 through RC-06 and the required Raspberry Pi lifecycle on suitable
   fresh hosts.
5. Complete RC-07, build only the frozen candidate artifacts, and assemble
   RC-08 from their exact hashes and retained evidence.
6. Perform a final adversarial release-candidate audit before any tag,
   publication, or release action.
