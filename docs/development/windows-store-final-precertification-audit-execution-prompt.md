# Microsoft Store final pre-certification audit execution prompt

## Objective

Perform a read-only, fail-closed reconciliation of Partner Center Submission 1
against the approved repository record after every required section reports
`Complete`. Identify every remaining optional input, stale statement, package
or listing mismatch, and submission-time consequence. Stop before selecting
**Submit for certification** or changing any Partner Center state.

## Verified context

- Submission 1 uses internal ID `1152921505701677071` and remains `In draft`.
- The accepted package is `SDRCalibration-0.1.1-Windows-Store-x64.msix`, version
  `0.1.1.0`, `X64`, `Windows.Desktop`, minimum version `10.0.22000.0`.
- The exact owner-bound package SHA-256 is
  `6d6998bb2130b9f137ac2847c8449f24259f5a526f1f8c67d66f7953f9327f08`.
- All six required submission sections report `Complete`; the package reports
  `Validated`; **Submit for certification** is enabled.
- Manual publication remains selected, so certification success must not
  publish until the owner later selects **Publish now**.
- The public privacy-policy URL and 478-character `runFullTrust` justification
  are saved. `Utilities + tools` is primary and owner-approved `Developer
  tools` is secondary.
- A deterministic supplemental certification fixture exists, but it was
  exercised against the superseded first candidate and has not been uploaded.

## Scope

1. Confirm the repository is clean, synchronized, and matches the recorded
   evidence revision before starting the audit.
2. Inspect the visible overview and every required section without saving,
   uploading, deleting, or changing a field.
3. Verify price, availability, audience, discoverability, schedule, category,
   privacy URL, declarations, age rating, package identity/version/
   architecture/targeting, listing language/text/assets, manual publication
   hold, and exact restricted-capability explanation.
4. Inspect Additional Testing Information and any other supplemental area to
   determine whether certification notes, credentials, or attachments are
   present, required, optional, stale, or unsafe.
5. Treat the retained certification fixture as stale for upload unless it is
   requalified against the accepted replacement package and separately
   authorized. Do not upload it in this slice.
6. Reconcile stale readiness prose and checkboxes with the observed final draft.
7. Produce an exact owner authorization statement for the later certification
   submission slice, but do not infer that authorization from this audit.

## Constraints and non-goals

- Do not select **Submit for certification**, accept any resulting warning, or
  initiate certification.
- Do not upload the fixture or any other attachment.
- Do not publish, change the manual publication hold, delete the submission or
  package, change targeting, or edit notification recipients.
- Do not enter credentials, contact details, addresses, or sensitive data.
- Do not build or modify a package, access SDR hardware, start a stream, or
  perform RF work.

## Validation and evidence

- Record authoritative visible values and distinguish them from inference.
- Record whether supplemental testing information is empty or populated and
  whether Partner Center requires it for submission.
- Verify the draft remains `In draft` after the audit.
- Run the Store source-contract verifier, targeted Store CTest, strict Sphinx
  build, and `git diff --check`.

## Adversarial review

Attempt to disprove readiness through stale candidate references, an invalid
privacy URL, omitted or truncated `runFullTrust` text, automatic publication,
wrong category, unexpected device family, incomplete section, stale fixture,
credential request, unsupported claim, accidental save, attachment upload,
certification action, or publication side effect. Correct every repository-only
finding and repeat affected checks until clean.

## Exit criteria

- The entire visible draft is reconciled or every unresolved item is explicit.
- Supplemental information and fixture disposition are truthful and safe.
- No Partner Center state changes, certification submission, or publication
  occur.
- Repository evidence passes all narrow checks and attributable changes are
  committed and pushed.

## Executed outcome

The read-only audit reconciled every required section with the approved packet.
Additional Testing Information contains an empty optional description, zero
credentials, and no attachment control. The superseded-candidate fixture
remains excluded. All six submission sections report `Complete`, the sole
Desktop package reports `Validated`, the manual publication hold remains
selected, and the product remains `In draft`. **Submit for certification** is
enabled and was not selected.
