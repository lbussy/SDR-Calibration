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
| RC-05 | Windows clean-host and public-trust lifecycle | Same-host self-signed lifecycle only | Fresh Windows 11 x64 host and an explicit public binary/signing decision | Exact MSI hash; trust mode, SmartScreen observations, lifecycle, and cleanup |
| RC-06 | Prior-version upgrade and rollback | Not qualified on macOS or Windows | Select an immutable prior-version baseline and fresh-host test arrangements | Old/new hashes, preserved user state, migration outcome, rollback procedure, and residue review |
| RC-07 | Initial compatibility freeze | Not started | Decide the initial native-profile/schema compatibility promise and supported artifact set | Frozen revision, schemas, compatibility statement, and change-control rule |
| RC-08 | Release documentation and evidence ledger | Not assembled for a frozen candidate | RC-07 plus exact final candidate artifacts; physical and clean-host results must be linked, not inferred | Release notes, known limitations, upgrade/rollback guidance, vulnerability reporting, checksums, inventories, and evidence index |

RC-03 is the accuracy-bearing gate. Package success, synthetic workflow success,
or capture success cannot substitute for it. RC-04 through RC-06 are
distribution lifecycle gates and cannot be satisfied by another same-host run.

## Scope decisions required before freeze

These are decisions, not automatically failed qualifications:

- Whether the initial release distributes a Windows binary. If it does, the
  accepted signing/trust model and clean-host evidence become release blockers.
- Whether Raspberry Pi recorded-input CLI is part of the initial released
  artifact set. If it is, its clean-host install/upgrade/removal gate joins the
  blocking ledger.
- Whether unsigned locally generated profiles are acceptable for the initial
  release with explicit limitations, or production signing and trust management
  must precede the release.
- Whether profile activation/evaluation orchestration and safe WSJT-X settings
  mutation are initial-release features or explicitly deferred non-goals.
- Which exact SDR/device/reference combinations, if any, will be named as
  supported rather than experimental or unqualified.

## Hardware-free work that may proceed

1. Draft the initial compatibility and artifact-scope decision for review,
   without freezing a revision.
2. Create release-note, known-limitations, upgrade/rollback, vulnerability-
   reporting, checksum, and evidence-ledger templates populated only with
   verified facts.
3. Design clean-host macOS and Windows lifecycle plans bound to exact candidate
   artifacts, but do not simulate a clean host on the development machine.
4. Define the prior-version baseline-selection rule and upgrade/rollback
   acceptance criteria.
5. Audit final icon rendering on Windows and any other platform included in the
   release artifact set; the current retained human review covers macOS only.

## Ordered resumption path

1. Complete RC-01 after the splitter is installed.
2. Complete RC-02 with the controlled reference path.
3. Run RC-03 for each exact device/reference/platform combination proposed for
   a positive support claim.
4. Resolve artifact-scope decisions and execute RC-04 through RC-06 on suitable
   fresh hosts.
5. Complete RC-07, build only the frozen candidate artifacts, and assemble
   RC-08 from their exact hashes and retained evidence.
6. Perform a final adversarial release-candidate audit before any tag,
   publication, or release action.
