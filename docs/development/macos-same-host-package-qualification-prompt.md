# macOS same-host package qualification prompt

## Objective

Produce and qualify one exact current-source macOS Apple Silicon package using
the existing official-Qt production path. Retain signed/notarized DMG evidence,
then perform a bounded same-host mount, copy/install, first launch, normal
launch, product-name/icon review, CLI startup, removal, and clean-state check.

This is an exact-host lifecycle result. It is not clean-host qualification and
does not close prior-version upgrade, other-platform, device, reference, or
calibration gates.

## Verified starting point

- The repository is clean and synchronized on `main` after the GPSDO readiness
  package.
- The configured production tree uses official Qt 6.11.1, disables SoapySDR,
  and retains the existing Developer ID identity and `sdrcal-notary` Keychain
  profile.
- Packaging implements exact-payload license disposition, hardened-runtime
  signing, application and DMG notarization, stapling, Gatekeeper assessment,
  mounted-app signature checks, and a bounded launch smoke.
- Earlier macOS packages apply only to their recorded source revision and
  artifact hash. They do not qualify the current candidate.

## Required execution

1. Bind the candidate to the clean prompt-containing `HEAD`; record OS,
   architecture, compiler, CMake, Qt, source revision, signing identity, and
   package configuration without exposing credentials.
2. Configure and build the existing `macos-official-release` tree, run all
   hardware-free tests, the staged package audit, and the production
   `macos-dmg` target without weakening any gate.
3. Independently verify the DMG SHA-256, evidence manifest, application and DMG
   signatures, notarization acceptance, stapling, Gatekeeper, runtime closure,
   exact-payload license disposition, bundle metadata, and icon resource.
4. Record pre-existing `/Applications/SDR Calibration.app` and relevant
   per-user application state before installation. Preserve an existing
   application rather than overwriting it; stop if safe isolation is not
   possible.
5. Mount the exact DMG and copy its application to `/Applications` only after
   verifying the mounted bundle matches the retained package evidence.
6. Launch the installed application for a bounded first-launch review. Verify
   the visible product name, main-window identity, expected controls, icon
   presence and legibility, and absence of unexpected warnings or permission
   prompts. Do not initiate live mode or select any device/input.
7. Quit and relaunch for a bounded normal-launch review; run the packaged CLI's
   non-mutating startup/help path.
8. Remove only the application installed by this plan, verify it is absent,
   detach the DMG, and confirm no application or helper process remains. Do not
   delete pre-existing user preferences, profiles, or unrelated files.
9. Retain a privacy-reviewed exact-host qualification record and update the
   platform/status documentation without calling the result clean-host or
   upgrade qualification.

## Safety boundary and non-goals

- Do not enumerate, open, configure, or stream from an SDR and do not perform
  RF-dependent work.
- Do not use live calibration, recorded calibration, or modify user profiles.
- Do not install dependencies, alter credentials, change Gatekeeper settings,
  bypass security warnings, publish a release, or upload artifacts beyond the
  existing notarization service required by the approved package target.
- Do not overwrite or remove a pre-existing application or user data.
- Do not claim clean-host installation, prior-version upgrade, Intel macOS,
  Windows, Raspberry Pi, device support, reference suitability, or calibration
  accuracy.

## Validation and adversarial review

Run the exact production build/test/package gates, independent artifact and
evidence checks, bounded installed-app lifecycle, Sphinx with warnings as
errors, and `git diff --check`.

Review for dirty-source packaging, stale artifact reuse, source/hash mismatch,
signing or notarization gaps, unstapled content, payload/license drift, absolute
runtime paths, stale `sdrcal-gui.app` naming, missing or misleading icons,
pre-existing-install overwrite, launch from the mounted rather than installed
copy, lingering processes or mounts, user-data deletion, and qualification
overclaim. Correct every actionable finding and repeat affected checks.

## Exit criteria

One exact DMG is production-gate-valid and independently identified; the exact
application passes a bounded same-host install/first-launch/normal-launch/CLI/
removal lifecycle; cleanup is complete; retained documentation states all
limitations; and the repository ends clean and synchronized after separately
authorized commit and push.
