# Upgrade and rollback qualification plan

Status: Prepared; no upgrade or rollback result is qualified

## Purpose and qualification boundary

This plan governs one exact transition from each retained `0.1.0` package to a
future frozen package whose version is strictly greater than `0.1.0`, followed
by restoration of the exact prior version. Results are platform-, architecture-,
source-, artifact-, signing-, host-, and procedure-specific. They do not
transfer to rebuilt packages or another platform.

Clean-host installation has been accepted separately. Upgrade qualification
must begin with the exact old package installed and operating; it is not a
clean-box installation and not a same-version maintenance installation.

## Exact prior baselines

| Platform | Artifact | Source | SHA-256 | Trust state |
| --- | --- | --- | --- | --- |
| macOS ARM64 | `SDRCalibration-0.1.0-macOS.dmg` | `5eb3670b08b8aee2c4d915e5553b140394dc0d01` | `94a831d4549b92edd85222c55e0cd64395dbe8acfd1f4bd5c57351f15bf80ad4` | Developer ID signed, notarized, stapled, Gatekeeper accepted |
| Windows x64 | `SDRCalibration-0.1.0-Windows-x64.msi` | `15ed17b8cb3e9ed75fa50bb407219cd2f3aaf193` | `7be98c92de35bb31024152161f9b3bb4e76cbf6a3d2728001060f3d338d81973` | Locally trusted self-signed development certificate; no public trust or timestamp |
| Raspberry Pi OS ARM64 | `sdrcal_0.1.0_arm64.deb` | `5f50d3190999ee647fa1b650ed39e46175a54fd9` | `d3de3d6397aaec7c3959e8854ac1871a787e4850706ae7736761a3fa887605ef` | No repository or package signing implemented |

The content-addressed retention paths and byte sizes remain authoritative in
the [baseline manifest](https://github.com/lbussy/SDR-Calibration/blob/main/evidence/release-baselines/0.1.0/README.md).

## Candidate entry gate

Stop before touching a test host unless all of the following are retained:

1. A clean, synchronized, frozen source revision with a project version
   strictly greater than `0.1.0`.
2. Exact platform package filename, size, SHA-256, source revision, build
   environment, signing/trust state, payload inventory, and license disposition.
3. Consistent version output across `project(... VERSION ...)`, exposed GUI/CLI
   version surfaces, macOS `CFBundleShortVersionString` and `CFBundleVersion`,
   Windows MSI product version, Debian package version, filenames, SBOM, and evidence.
   The current hard-coded GUI `0.1.0` prevents this gate from passing until a
   separately reviewed implementation slice removes that drift risk.
4. Candidate package gates passed independently. Windows transition testing
   records the exact trust mode; self-signed mechanics never satisfy the
   separate public-trust release gate.
5. A named test host with supported OS/architecture, stable power, sufficient
   storage, no pending reboot, no active SDR Calibration process, and explicit
   operator ownership of the maintenance window.
6. Verified access to both old and new artifacts plus a recovery copy of the
   old artifact that does not depend on the application being operational.

Any identity, hash, signature, version, host-state, backup, or ownership
ambiguity is a fail-closed stop.

## Shared state fixture and inventory

Create a uniquely named test directory outside every installation root. Seed
it with synthetic, non-sensitive examples of:

- a recorded calibration request;
- a native calibration profile;
- an evidence directory and summary;
- a registry signature pin;
- a small recorded CF32LE input and its manifest; and
- an unrelated sentinel file.

Record relative paths, sizes, modes/attributes, and SHA-256 values in a sorted
manifest generated independently of the application. These are user-owned
external files, not an application-managed store. The package transition must
not rewrite, move, delete, adopt, or silently migrate them.

Before upgrade, inventory the installed payload, application/version identity,
package-manager registration, launch result, running processes, relevant
preferences if any exist, and the complete seeded-state manifest. Back up any
state that the platform procedure could affect, independently verify the backup
manifest, and record the restoration path.

## Shared upgrade acceptance

An upgrade passes only if all of these are true:

- the old installation was first proven to be the exact retained `0.1.0` input;
- the documented platform-native transition completed without an unreviewed
  error, repair, fallback, or manual payload substitution;
- only the new package is registered and its installed payload and visible
  version match the exact candidate;
- GUI/CLI scope-appropriate launch checks pass without missing dependencies or
  unexpected security, permission, migration, or repair prompts;
- the seeded-state manifest is byte-for-byte unchanged;
- no unexpected service, scheduled task, startup item, process, permission,
  installation copy, package registration, or unrelated file appears; and
- all commands, installer logs, inventories, hashes, observations, aborts, and
  cleanup status are retained.

## Platform procedures

### macOS ARM64 DMG

1. Verify the retained old DMG hash, signature, notarization/staple, and
   Gatekeeper result; mount it and copy the old `SDR Calibration.app` to
   `/Applications` only after confirming no conflicting copy or process exists.
2. Record the installed bundle identifier `com.lbussy.sdrcal`, executable hash,
   bundle versions, CLI version/help behavior, Gatekeeper result, and launch.
3. Verify and mount the exact candidate DMG. Quit the old application and use
   the documented Finder-equivalent bundle replacement into `/Applications`;
   do not merge bundle contents or copy individual files.
4. Verify the new complete bundle, signature, Gatekeeper result, bundle
   identifier/version, executable hash, first launch, normal relaunch, CLI, and
   seeded-state preservation.
5. For rollback, quit the new version, remove only its exact installed bundle,
   restore the exact old bundle from the verified old DMG, and repeat old
   identity, signature, launch, CLI, and seeded-state checks.
6. Review `/Applications`, mounted images, processes, relevant preference files,
   and the seeded-state directory. Record whether the final host intentionally
   retains `0.1.0` or is returned to its pre-test state.

### Windows 11 x64 MSI

1. Verify the old MSI hash and Authenticode state, install it through Windows
   Installer, and record the MSI product registration, stable `UpgradeCode`,
   installed payload hashes, Start menu entry, application version, launch,
   and CLI behavior.
2. Verify the exact candidate MSI hash/signature and retain a verbose MSI log.
   Install it through Windows Installer. The stable `UpgradeCode` and WiX
   `MajorUpgrade` path must remove the old product and leave exactly one new
   registered product; manual file replacement is forbidden.
3. Verify new product registration, version, payload, shortcut, launch, CLI,
   seeded-state preservation, processes, services, scheduled tasks, registry
   residue, and installer result/reboot state.
4. WiX intentionally rejects installing the older MSI over a newer product.
   Rollback therefore requires a logged uninstall of the exact new MSI,
   confirmation that its product registration and installed payload are gone,
   then a logged reinstall of the exact retained old MSI.
5. Reverify old product registration, trust state, payload hashes, shortcut,
   launch, CLI, seeded state, and residue. Public-trust/SmartScreen conclusions
   are recorded only when the exact candidate uses that separately qualified
   signing mode.

### Raspberry Pi OS 13 ARM64 CLI DEB

1. Verify the retained old DEB hash and `dpkg-deb` control identity, install it
   with the documented package-manager path, and record `dpkg-query` status,
   installed payload hashes, CLI version/help, dependencies, and processes.
2. Verify the exact candidate DEB hash/control identity, apply it through
   `dpkg`, and retain complete output and post-install package/payload state.
   Do not substitute extracted files for a package transition.
3. Verify the new version, payload, CLI, dependencies, seeded-state
   preservation, package-manager health, and absence of unexpected services,
   users, groups, permissions, or files.
4. Roll back by deliberately installing the exact retained `0.1.0` DEB through
   `dpkg`; retain the downgrade diagnostic and exit status rather than assuming
   success from package state alone.
5. Reverify `0.1.0` package status, payload hashes, CLI, dependencies, seeded
   state, residue, and the intended final installed/removed state.

## Failure, abort, and cleanup rules

- Stop on any hash, signature/trust, version, architecture, package identity,
  seeded-state, backup, or test-host mismatch.
- Do not improvise by deleting package-manager metadata, disabling platform
  security, editing the registry/package database, force-overwriting payloads,
  or reconstructing an artifact.
- A failed upgrade does not authorize rollback unless the installed state is
  known well enough to follow the reviewed recovery path safely.
- If cleanup cannot establish the final installed version, package registration,
  running-process state, seeded-state integrity, and residue inventory, record
  `cleanup_failed`; do not claim either upgrade or rollback qualification.
- Preserve all logs and partial evidence, including failed attempts.

## Evidence and independent review

Create one timestamped record per platform from the
[upgrade and rollback template](release/upgrade-and-rollback-template.md).
Retain host identity, old/new artifact identities, version-consistency report,
state and backup manifests, commands, installer/package-manager logs, before/
after payload inventories, launch observations, seeded-state comparisons,
abort/cleanup results, and reviewer disposition.

An independent review must recompute artifact and state hashes, reconcile
package-manager state with payload state, confirm the correct platform
procedure, and challenge overclaims. Any actionable finding reopens the result.

## Exit rule

Qualification is per platform and exact candidate. `Passed` requires both the
upgrade and rollback procedures plus final cleanup to pass. `Failed` retains
all evidence and grants no partial platform claim. `Not run` and `Blocked` are
not qualification. No aggregate RC-06 pass exists until all intended release
platforms have passing, independently reviewed records for the same frozen
candidate.
