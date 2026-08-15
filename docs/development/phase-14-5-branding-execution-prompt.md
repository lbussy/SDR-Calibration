# Phase 14.5 branding pre-qualification execution prompt

## Objective

Establish the approved `SDR Calibration` product identity and project-owned icon
across the macOS, Windows, and Linux GUI package definitions before any new
clean-host qualification candidate is produced. Preserve `sdrcal-gui` as the
internal CMake target and executable basename. This is a hardware-free,
network-free, pre-qualification implementation slice.

## Verified starting point

- Commit `b067e52` records the cross-platform GUI branding requirement.
- Commits `b3fc511` and `ef62635` add the approved source masters, reproducible
  generator, native icon assets, and their initial build/package wiring.
- macOS embeds `SDRCalibration.icns` and declares `CFBundleName` as
  `SDR Calibration`, but the installed bundle and package paths still use
  `sdrcal-gui.app`.
- Windows embeds `SDRCalibration.ico` and declares the MSI product and install
  directory as `SDR Calibration`, but the package definition does not yet
  establish or verify a user-facing application shortcut.
- Linux declares `Name=SDR Calibration`, `Exec=sdrcal-gui`, and
  `Icon=sdr-calibration`, and installs hicolor PNG assets, but this contract
  needs deterministic source-tree and staged-package verification.
- Existing signed, notarized, extracted, or retained packages predate the final
  cross-platform identity and remain historical evidence only.

## Required implementation

1. Install the macOS bundle as `SDR Calibration.app` while keeping the internal
   executable at `Contents/MacOS/sdrcal-gui`. Update every macOS staging,
   deployment, runtime-inventory, license-disposition, signing, notarization,
   stapling, Gatekeeper, mounted-launch, and evidence path to the spaced bundle
   name. Quote every filesystem expansion that can contain the space.
2. Preserve the stable macOS bundle identifier and confirm `CFBundleName`,
   `CFBundleExecutable`, and `CFBundleIconFile` resolve to the intended product
   name, internal executable, and installed icon.
3. Keep the Windows executable basename `sdrcal-gui.exe`, MSI product name,
   feature title, and install directory aligned with `SDR Calibration`. Add a
   Start menu application shortcut named `SDR Calibration` targeting the GUI
   executable and using the project icon. Give the shortcut deterministic
   install/uninstall ownership and verify its generated package definition.
4. Verify the Linux desktop entry uses `Name=SDR Calibration`,
   `Exec=sdrcal-gui`, `Icon=sdr-calibration`, and `Terminal=false`; verify every
   declared hicolor icon resolves to a nonempty PNG with the correct dimensions
   in both source and applicable staged-package audits.
5. Add durable icon provenance covering project ownership, authorship/source,
   approval date, MIT licensing, editable committed masters, generator path,
   conversion dependency/version, platform representations, and limitations.
   Add a machine-readable SHA-256 manifest for both masters, the generator, and
   every generated native asset.
6. Add deterministic, hardware-free branding tests that fail closed on stale
   `sdrcal-gui.app` package paths, inconsistent product names, missing shortcut
   or icon bindings, unresolved Linux desktop icons, invalid native icon
   containers, incorrect PNG dimensions, or hash-manifest drift.
7. Update status, roadmap, testing, packaging, user, and phase documentation to
   distinguish this implemented identity slice from clean-host, signing,
   device, calibration, and release qualification.

## Validation

Run the narrowest applicable hardware-free checks:

1. Icon/provenance and cross-platform branding contract tests.
2. `cmake --preset default`.
3. `cmake --build --preset default`.
4. `ctest --preset default`.
5. The local staged `package-audit` target.
6. A macOS release configure/build/test/staged-package audit when its existing
   preset and dependencies are available, without invoking the signed DMG
   target.
7. PowerShell parser validation for the Windows package script when PowerShell
   is available; otherwise report it as skipped and rely only on the static
   branding contract test, not a Windows package claim.
8. Shell syntax validation for macOS and Ubuntu packaging scripts.
9. `sphinx-build -W -b html docs docs/_build/html`.
10. `git diff --check` and a complete diff review.

Do not claim a platform-native package passed when the current host cannot
construct it. Report unavailable tools and platform-only checks explicitly.

## Safety boundary and non-goals

- Do not enumerate, open, configure, or stream from SDR hardware and do not
  perform RF-dependent testing.
- Do not sign, notarize, staple, publish, upload, or release a new package.
- Do not perform clean-host install, upgrade, uninstall, or clean-user-state
  qualification.
- Do not access credentials, Keychain profiles, certificates, timestamp
  services, or external services.
- Do not rename the CLI, native calibration profile, repository, bundle
  identifier, CMake target, or internal GUI executable.
- Do not claim device support, calibration accuracy, clean-host support, or
  completion of all Phase 14.5 gates.

## Adversarial exit gate

After implementation and ordinary validation, review the complete diff,
installed staging tree, and retained outputs adversarially for stale product
paths, unquoted spaces, inconsistent platform identity, shortcut ownership or
uninstall defects, icon lookup failures, missing/incorrect representations,
hash or provenance gaps, generated-artifact drift, build-tree leakage,
dependency/license omissions, false qualification claims, hidden hardware or
network activity, and validation blind spots.

Treat every actionable finding as a blocker. Correct it, rerun every affected
check, and repeat the adversarial review until no actionable finding remains.
Report only after that clean exit. Stop with the changes uncommitted unless the
user separately authorizes commit and push.

## Execution result

The bounded implementation is complete in the working tree. The macOS staged
bundle is `SDR Calibration.app` with internal executable `sdrcal-gui`; all DMG,
signing, notarization, Gatekeeper, inventory, launch, and replacement-guidance
paths use the spaced bundle name. The Windows WiX definition retains
`sdrcal-gui.exe` internally and defines an icon-bearing `SDR Calibration` Start
menu shortcut plus installed-app icon. The Linux desktop identity and hicolor
resolution are enforced. Icon ownership, MIT disposition, retained masters,
generator and conversion versions, and SHA-256 records travel in the staged
payload and are checked deterministically.

No signed DMG or MSI, Ubuntu DEB, clean-host lifecycle, hardware access, or
publication was performed. PowerShell and WiX were unavailable on the macOS
host, so Windows validation is limited to source-contract and well-formed WiX
XML checks; it is not a native MSI result. Ubuntu package construction remains
an Ubuntu-only later check. Human visual-review evidence and exact candidate
hashes remain open before clean-host qualification.

## Injected adversarial findings

1. **The first stale-path test rejected its own safety assertion:** the test
   treated the package audit's deliberate rejection of `sdrcal-gui.app` as an
   active stale package path. Corrected by allowing exactly one occurrence only
   in the explicit fail-closed negative assertion and rejecting it everywhere
   else in active packaging paths.
2. **The Windows shortcut icon identifier did not follow the WiX icon contract:**
   `SDRCalibrationIcon` lacked the source file's `.ico` extension. Corrected to
   `SDRCalibration.ico` for the Icon table, shortcut, and `ARPPRODUCTICON`, then
   rechecked against the WiX v4 schema contract.
3. **The initial Windows verification was marker-only:** it did not prove that
   the generated WiX template was well-formed XML or that the `.ico` resource
   entered the GUI target. Corrected by parsing the rendered template and
   checking the target, name, icon, executable resource, installed-app icon,
   component ownership, registry key path, and uninstall cleanup contract.
4. **Staged provenance could drift from the reviewed source manifest:** required
   filenames alone did not prove the package carried the exact reviewed icon
   evidence. Corrected with source-to-stage manifest hash/equality checks in the
   common package audit and all three native package scripts.
5. **Linux resolution checks were too indirect:** the first test looked only for
   size numerals. Corrected by checking the exact CMake source and hicolor
   destination patterns, every required source PNG and dimension, and the
   desktop entry's name, executable, icon key, and terminal policy.
6. **The operator documentation omitted Windows package identity:** the user
   guide described macOS and Ubuntu packaging but did not expose the Windows
   installed-product and Start menu contract or its qualification limits.
   Corrected with an explicit Windows status section and corresponding Linux
   launcher-resolution wording.

After these corrections, the repeated adversarial review found no remaining
actionable issue within this bounded pre-qualification slice.
