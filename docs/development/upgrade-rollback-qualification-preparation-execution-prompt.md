# Upgrade and rollback qualification preparation execution prompt

## Objective

Prepare a complete, hardware-free upgrade and rollback qualification contract
for the intended initial macOS ARM64 DMG, Windows 11 x64 MSI, and Raspberry Pi
OS 13 ARM64 CLI DEB. Bind the plan to the retained, content-addressed `0.1.0`
baselines, define exact platform procedures and fail-closed acceptance criteria,
and make the future evidence reviewable before a higher-version candidate is
created.

This slice prepares qualification. It does not increment a version, build or
install a package, execute an upgrade or rollback, qualify a result, or publish
an artifact.

## Verified context

- Exact read-only `0.1.0` baselines are retained for all three platforms with
  full SHA-256 identities in the baseline manifest.
- Owner-confirmed clean-host lifecycle passes exist for the exact indexed
  macOS DMG, self-signed Windows MSI, and Raspberry Pi DEB. Clean-host evidence
  does not substitute for a version-to-version transition.
- The repository still declares project version `0.1.0`; the GUI also embeds
  `0.1.0` directly. A future candidate must be strictly newer and must expose a
  consistent version across source, executable, bundle, installer/package,
  evidence, and user-visible surfaces.
- Windows packaging uses a stable WiX `UpgradeCode` and `MajorUpgrade`, rejects
  downgrade installation, and currently has only self-signed evidence. Public-
  trust signing is a separate workstream.
- macOS uses an application bundle copied to `/Applications`; Windows uses a
  per-machine MSI under `Program Files`; Raspberry Pi uses package name
  `sdrcal` and a CLI-only DEB.
- The application does not currently own a production profile store or mutate
  WSJT-X. Calibration profiles, evidence, requests, trust pins, and recorded IQ
  are user-selected external inputs or outputs and must never be silently
  deleted or rewritten by package transitions.

## Scope and required work

1. Create a durable qualification plan defining shared preconditions, evidence,
   abort rules, backup/restore requirements, seeded-state checks, platform-
   native upgrade paths, rollback paths, residue review, and pass/fail rules.
2. Bind the prior side of every transition to the exact retained `0.1.0`
   filename, source revision, SHA-256, signing state, and content-addressed
   location already recorded in the baseline manifest.
3. Require the future candidate to have a version strictly greater than
   `0.1.0`, a clean frozen source revision, exact package hash, complete package
   and license gates, and internally consistent version identity. Treat the
   hard-coded GUI version as a pre-candidate blocker; do not fix or increment it
   in this slice.
4. Define a shared seeded-state fixture outside installation roots containing
   representative request, profile, evidence, trust-pin, and recorded-input
   files with an independently generated manifest. Require byte-for-byte
   preservation through upgrade and rollback. Do not imply that the application
   manages these files.
5. Define platform-specific procedures:
   - macOS: install the exact old bundle, verify identity, replace it only via
     the documented mounted-DMG copy path, validate the new bundle, remove it,
     restore the exact old bundle, and review `/Applications` plus relevant
     user state for residue.
   - Windows: install the exact old MSI, inventory product/upgrade identity,
     apply the exact higher-version MSI through Windows Installer, verify the
     MajorUpgrade result, then uninstall the new MSI before reinstalling the
     older MSI because downgrades are intentionally blocked. Keep public trust
     and transition mechanics distinct.
   - Raspberry Pi: install the exact old ARM64 DEB, inventory `dpkg` state and
     payload, apply the exact higher-version DEB, then deliberately reinstall
     the retained old DEB as a downgrade while retaining full command output,
     package state, payload hashes, and cleanup evidence.
6. Upgrade the release lifecycle template so a future execution record cannot
   omit exact candidate identity, version-consistency evidence, seeded-state
   hashes, platform procedure, abort/cleanup, residue review, or independent
   review.
7. Update active readiness/status/navigation documentation to mark preparation
   complete while leaving actual upgrade/rollback qualification open.

## Constraints and non-goals

- Do not change `PROJECT_VERSION`, the GUI version, schema versions, bundle or
  package identities, WiX `UpgradeCode`, or installer behavior.
- Do not build, copy, install, remove, upgrade, downgrade, sign, notarize,
  timestamp, launch, or publish a package.
- Do not access another host, external service, SDR, reference, antenna,
  splitter, signal generator, GPSDO, or RF path.
- Do not invent a production application-data store, migration, compatibility
  promise, or rollback capability not established by the source.
- Do not qualify Windows public trust, SmartScreen reputation, calibration
  accuracy, device support, clean-host behavior for a rebuilt artifact, or a
  release candidate.
- Preserve the user's existing clean-host documentation changes and all other
  worktree content.

## Validation and retained evidence

- Review current CMake, GUI, macOS bundle, WiX, Debian, baseline, compatibility,
  and release-template contracts against every procedure.
- Confirm every retained baseline filename, source revision, SHA-256, trust
  state, and platform is transcribed without alteration.
- Run `git diff --check` and the Sphinx warnings-as-errors build.
- Review the complete diff and staged diff before committing. No binary,
  generated HTML, package, credential, or host-specific secret may be staged.

## Adversarial review

Challenge the result for same-version maintenance mislabeled as upgrade;
clean-host evidence substituted for transition evidence; Windows downgrade
attempted through a path that WiX rejects; self-signed trust promoted to public
trust; macOS bundle replacement treated as data migration; Debian downgrade
assumed without retained `dpkg` evidence; unowned user files treated as managed
state; missing backup verification; stale or inconsistent versions; artifact
hash drift; partial cleanup; rollback declared after only uninstalling the new
version; platform evidence transferred to another package; or qualification
claimed before execution. Correct every actionable finding and repeat affected
checks until none remain.

## Exit criteria

- The execution prompt and durable platform plan are complete and linked.
- The lifecycle template is execution-ready for each intended platform.
- Exact `0.1.0` baselines and future-candidate preconditions are explicit.
- The version-consistency blocker and Windows signing boundary are explicit.
- Active status says preparation is complete and execution remains open.
- Documentation validation and diff checks pass.
- Only attributable changes are committed and pushed to the current branch;
  no candidate, transition, signing, hardware, or release action occurs.

## Execution result

Completed on 2026-08-17 as a hardware-free documentation and qualification-
design slice. The durable plan, expanded execution record template, navigation,
readiness ledger, roadmap, status, clean-host record, and platform matrix were
updated. No version, package, host, signing, hardware, or external-service
operation occurred.

The adversarial review corrected an initially shortened Windows MSI filename to
the exact retained `SDRCalibration-0.1.0-Windows-x64.msi` identity and removed
the stale claim that Windows application/icon review remained open after owner
acceptance. It also made the hard-coded GUI `0.1.0` a fail-closed candidate
entry blocker, separated Windows public trust from upgrade mechanics, required
explicit uninstall/reinstall for WiX rollback, and prohibited treating external
user artifacts as application-managed state. After correction, no actionable
finding remained within this preparation slice.
