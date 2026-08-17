# Microsoft Store pre-upload execution prompt

## Objective

Establish the exact Microsoft Store MSIX upload candidate and reconcile the
owner-authorized Partner Center draft state without uploading any file or
requesting certification or publication.

## Verified context

- The active candidate is version `0.1.1`, source revision
  `957fbeb204177c9ba2a1582e936476244b201b9d`, and unsigned MSIX SHA-256
  `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.
- Partner Center `Submission 1` exists and remains in draft.
- Pricing, Properties, Age ratings, and Submission options have owner-approved
  saved values. Package upload and listing completion remain open.

## Scope

1. Inspect the complete local worktree before editing.
2. Recompute the candidate hash on `ecm-mule` and compare its embedded evidence
   manifest and unpacked `AppxManifest.xml` with the active readiness packet.
3. Inspect the live Partner Center Packages page without selecting, saving, or
   uploading anything.
4. Record the exact candidate path, bindings, current Partner Center state,
   completed owner approvals, and remaining decisions in project evidence.
5. Update deterministic source-contract checks for the new durable state.

## Constraints and non-goals

- Do not upload a package, fixture, screenshot, or listing asset.
- Do not select or save device-family availability.
- Do not accept a new agreement or attestation.
- Do not request package validation, certification, signing, delivery, or
  publication.
- Keep the manual publication hold intact.
- Do not access SDR hardware, start streams, or perform RF work.
- Do not infer approval for a support channel, future-device-family choice, or
  exact upload binding that the owner has not explicitly confirmed.

## Validation and evidence

- Retain the candidate filename, Windows path, size, SHA-256, source revision,
  identity, publisher, version, architecture, target device family, and sole
  restricted capability.
- Record all five visible device-family controls and the current
  future-device-family default.
- Run the Windows Store source-contract verifier, its targeted CTest, strict
  Sphinx build, and Git diff checks.

## Adversarial review

Review the complete diff for obsolete candidate confusion, inferred owner
approval, accidental upload claims, automatic-publication exposure, sensitive
account data, and language implying Store certification or signing. Correct
every actionable finding and reassess until clean.

## Exit criteria

- The exact candidate is unambiguously identified and independently rehashed.
- Partner Center package controls are recorded without state change.
- Completed and remaining owner gates are explicit.
- No upload, certification, or publication action occurred.
- Applicable checks pass, and only attributable files are committed and pushed
  when authorized.
