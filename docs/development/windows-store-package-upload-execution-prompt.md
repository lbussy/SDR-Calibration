# Microsoft Store package-upload execution prompt

## Objective

Upload the single owner-approved exact MSIX to Partner Center draft Submission
1, reconcile package validation and device-family results, and stop before any
certification-fixture upload, certification submission, or publication.

## Verified context

- Store listings, Pricing and availability, Properties, Age ratings, and
  Submission options are `Complete`; Packages is `Incomplete`.
- The intended upload is exactly
  `C:\Users\lee\SDR-Calibration-Harness-70ff94c\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`.
- Approved bindings are source revision
  `957fbeb204177c9ba2a1582e936476244b201b9d`, package version `0.1.1.0`, x64,
  and SHA-256
  `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.
- Windows 10/11 Desktop only is saved; Mobile, Xbox, Team, Mixed Reality, and
  automatic future-device-family availability are disabled.
- Manual publication remains required.

## Scope

1. Immediately before transfer, recompute the SHA-256 at the exact approved
   Windows path and refuse a mismatch.
2. Copy only that file to a new private temporary path on the controlling host;
   recompute its SHA-256 and refuse any cross-host mismatch.
3. Re-open Packages and verify no package is currently present and the approved
   device-family state is unchanged.
4. Select exactly the reverified temporary MSIX and wait for Partner Center's
   validation result.
5. Reconcile visible identity, version, architecture, device-family targeting,
   validation errors/warnings, package count, and section status with the
   owner-decision and readiness packets.
6. Save only if Partner Center requires a Save action for the valid package and
   approved device-family state.
7. Remove the temporary local transfer copy after durable evidence is recorded.

## Constraints and non-goals

- Do not select any same-named MSIX from another path or accept any hash drift.
- Do not upload the certification fixture, screenshots, logos, trailers,
  promotional art, or any other attachment.
- Do not change the approved Desktop-only or future-family selections.
- Do not select **Submit for certification**, publish, or enable automatic
  publication.
- Do not claim Microsoft signing, certification, Store delivery, clean-host
  installation, or update qualification from upload validation alone.
- Do not access SDR hardware, start a sample stream, or perform RF work.

## Validation and evidence

- Retain both immediate pre-transfer and post-transfer SHA-256 results, exact
  source path, byte size, package version, architecture, validation result, and
  visible device-family mapping without retaining account-sensitive material.
- Verify Store listings remains complete, Packages reaches the truthful status
  reported by Partner Center, and **Submit for certification** is not selected.
- Run the Store contract verifier, targeted Store contract test, strict Sphinx
  build, and `git diff --check`.

## Adversarial review

Inspect for wrong-path transfer, same-name confusion, hash drift, unexpected
package identity/version/architecture/capability, device-family changes,
validation warnings treated as success, stale no-upload language, accidental
fixture upload, and any certification/publication action. Correct every
actionable finding and reassess until clean.

## Exit criteria

- The uploaded package is byte-for-byte the owner-approved candidate.
- Partner Center validation and device-family mapping are reconciled and
  recorded truthfully.
- No fixture, certification, or publication action occurs.
- Temporary transfer material is removed, checks pass, and attributable
  repository changes are committed and pushed.
