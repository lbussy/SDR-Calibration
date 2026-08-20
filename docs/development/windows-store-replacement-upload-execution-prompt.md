# Microsoft Store replacement-package upload execution prompt

## Authorization gate

Do not execute any file selection or upload from this prompt until the owner
explicitly authorizes selecting and uploading the exact replacement package.
The recorded artifact-binding approval does not provide that authorization.
Certification submission and publication require later, separate approvals.

## Objective

After explicit upload authorization, upload the single owner-bound replacement
MSIX to Partner Center draft Submission 1, reconcile its validation result and
device-family mapping, and stop before any fixture upload, certification
submission, or publication action.

## Verified context

- The first uploaded `0.1.1` package was rejected and owner-authorized for
  deletion. The draft subsequently contained zero packages.
- Store listings and the previously saved non-package sections are complete;
  Packages remains incomplete.
- The exact replacement is:
  `C:\Users\lee\SDR-Calibration-Store-0.1.1-h-4406a82\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`.
- Its approved binding is source revision
  `4406a82e01072afc0d61d2516c2fe9607c608ea4`, package version `0.1.1.0`, x64,
  byte size `95910402`, and SHA-256
  `6d6998bb2130b9f137ac2847c8449f24259f5a526f1f8c67d66f7953f9327f08`.
- The replacement passed 19 hardware-free tests, package audit, MakeAppx
  pack/unpack, exact language/publisher/logo inspection, and the same-host
  development lifecycle.
- Windows 10/11 Desktop only is saved. Mobile, Xbox, Team, Mixed Reality, and
  automatic future-device-family availability are disabled.
- Manual publication remains required.

## Scope after authorization

1. Confirm the repository record is clean and synchronized and the replacement
   binding still has explicit owner approval.
2. Immediately before transfer, recompute SHA-256 and byte size at the exact
   Windows path. Refuse any mismatch.
3. Copy only those verified bytes to a newly created private temporary path on
   the controlling host. Recompute hash and size and refuse any transfer drift.
4. Reopen Partner Center Packages and verify zero package entries, Desktop-only
   targeting, future-family disabled, and no unexpected draft-state change.
5. Open the file picker only after all preceding gates pass and select exactly
   the isolated transfer copy.
6. Wait for Partner Center validation to finish. Record every visible error,
   warning, identity, version, architecture, targeting result, package count,
   and section status without treating warnings as success.
7. If accepted, use Save only when Partner Center requires it to retain the
   accepted package and unchanged approved targeting.
8. If rejected, do not delete or replace the rejected entry without a new
   explicit deletion authorization. Preserve the precise rejection evidence.
9. Remove the controlling-host temporary copy after evidence is durable;
   preserve the bound source package on Windows.

## Constraints and non-goals

- Do not select a same-named package from any other path.
- Do not upload the certification fixture, screenshots, logos, trailers,
  promotional art, symbols, or any other attachment.
- Do not change device-family or future-family selections.
- Do not select **Submit for certification**.
- Do not publish, enable automatic publication, or modify submission options.
- Do not delete a rejected cloud package without explicit authorization.
- Do not claim Store signing, certification, delivery, clean-host behavior, or
  resolution of the first rejection until Partner Center reports the relevant
  result.
- Do not access SDR hardware, start a sample stream, or perform RF work.

## Validation and evidence

- Record both immediate Windows and post-transfer hash/size results, exact
  paths, source revision, version, and architecture.
- Record the visible Partner Center package validation and device-family state
  without account-sensitive data.
- Verify that no fixture, certification, or publication action occurred.
- Run the Store source-contract verifier, targeted CTest, strict Sphinx build,
  and `git diff --check` for repository evidence changes.

## Adversarial review

Attempt to disprove the upload binding through path ambiguity, stale source,
same-name confusion, hash or size drift, manifest identity/version mismatch,
unexpected package count, device-family changes, unresolved logo/publisher/
language errors, warning suppression, accidental attachment selection, or any
certification/publication side effect. Correct all actionable findings and
repeat affected checks until clean.

## Exit criteria

- Explicit selection/upload authorization was obtained before opening a file
  picker.
- The selected bytes exactly match the approved replacement binding.
- Partner Center's complete validation result is recorded truthfully.
- Desktop-only targeting remains unchanged.
- No fixture, certification, deletion, or publication action occurs without its
  own authorization.
- Temporary transfer bytes are removed, repository checks pass, and only
  attributable evidence changes are committed and pushed.
