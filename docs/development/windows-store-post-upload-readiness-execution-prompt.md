# Microsoft Store post-upload readiness execution prompt

## Objective

Reconcile the Partner Center draft immediately after acceptance of the exact
replacement MSIX, complete only fields already covered by explicit owner
approval, expose every newly package-derived requirement, and stop before any
unapproved privacy-policy entry, certification submission, or publication.

## Verified context

- Partner Center accepted and saved the owner-bound replacement package as
  `0.1.1.0`, `X64`, for Windows Desktop with minimum version `10.0.22000.0`.
- Pricing and availability, Age ratings, Packages, and Store listings report
  `Complete`; Properties and Submission options report `Incomplete`.
- Desktop-only targeting and the manual publication hold are owner-approved.
- The owner-approved `runFullTrust` justification is retained in
  `windows-store-submission-readiness.md`.
- After package acceptance, Properties selects **Yes** for personal information
  based on declared capabilities and says a privacy policy is required. The
  form offers either a valid privacy-policy URL or directly entered policy text.

## Scope

1. Confirm the repository is clean and synchronized and the submission remains
   an unpublished draft.
2. Inspect every incomplete section without changing state first.
3. In Submission options, preserve **Don't publish this submission until I
   select Publish now**, enter only the exact approved `runFullTrust`
   justification, save, and verify the section result.
4. In Properties, preserve the approved category, support URLs, declarations,
   empty sensitive contact fields, and unspecified hardware requirements.
5. Do not change the package-derived privacy answer or provide a policy until
   the owner approves one exact URL or exact policy text after reviewing the
   newly visible requirement.
6. Return to the overview and record every section status and the state of
   **Submit for certification**.
7. Update durable evidence and the owner-decision packet with the exact new
   privacy gate and a concise approval statement for the next slice.

## Constraints and non-goals

- Do not upload the certification fixture or any other attachment.
- Do not submit for certification, publish, delete the draft or package, alter
  device-family targeting, or enable automatic publication.
- Do not invent, host, or enter privacy-policy text or a URL without explicit
  owner approval.
- Do not enter contact information, credentials, addresses, notification
  recipients, or other sensitive data.
- Do not modify package bytes, build a replacement, access SDR hardware, start
  a sample stream, or perform RF work.

## Validation and evidence

- Record the exact package-derived privacy message and available policy modes.
- Verify the saved manual publication hold and exact `runFullTrust` text.
- Record the overview section statuses and certification-button state.
- Run the Store source-contract verifier, targeted Store CTest, strict Sphinx
  build, and `git diff --check`.

## Adversarial review

Attempt to disprove completion through stale section status, a changed
publication hold, altered targeting, missing or paraphrased capability text,
privacy overclaim, invented legal language, sensitive-data entry, accidental
attachment upload, certification submission, or publication. Correct every
actionable finding and repeat affected checks until clean.

## Exit criteria

- Every already-authorized post-upload field is saved and verified.
- The newly mandatory privacy-policy choice is recorded without being guessed.
- No certification, publication, deletion, attachment upload, or sensitive-data
  entry occurs.
- Repository evidence is truthful, checks pass, and attributable changes are
  committed and pushed.
