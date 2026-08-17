# Microsoft Store listing-text execution prompt

## Objective

Reconcile and save the already-reviewed English (United States) Store-listing
text in Partner Center without selecting or uploading any file and without
requesting certification or publication.

## Verified context

- Product `SDR Calibration`, Store ID `9PK9W5S2D66L`, has draft Submission 1.
- Owner gates are complete; Pricing and availability, Properties, Age ratings,
  and Submission options are complete.
- Packages is incomplete with no uploaded package. The English (United States)
  listing is incomplete and requires at least one screenshot.
- The authoritative listing copy is in
  `windows-store-submission-readiness.md`.

## Scope

1. Inspect the complete visible English (United States) listing form.
2. Enter the approved description, eight separate product features, short
   description, and five approved keywords.
3. Leave What's new blank because this is the first submission.
4. Enter `Copyright (c) 2026 Lee Bussy` and `Lee Bussy` in the visible
   copyright and developer fields, matching the repository license and owner.
5. Leave Xbox-only fields, additional license terms, optional images, trailers,
   and all other unsupported or unapproved fields blank.
6. Save and re-open the listing to verify the visible persisted values.
7. Record the resulting state and remaining screenshot gate in repository
   evidence and readiness documentation.

## Constraints and non-goals

- Do not open a file picker or upload screenshots, logos, trailers, packages,
  fixtures, or attachments.
- Do not alter product name, pricing, availability, Properties, age ratings,
  package device families, or submission options.
- Do not select **Submit for certification**, publish, enable automatic
  publication, or make any other external-service change.
- Do not add marketing, hardware-support, accuracy, qualification,
  certification, telemetry, or privacy claims beyond implemented evidence.
- Preserve the native-profile authority and recorded-input-only boundary.

## Validation and evidence

- Verify the saved description and each feature against the committed source
  copy, including the offline and lossy-adapter qualifications.
- Verify What's new, Xbox-only fields, additional license terms, and all upload
  areas remain empty.
- Verify the listing remains incomplete solely because no screenshot is
  uploaded, Packages remains incomplete, and **Submit for certification** is
  disabled.
- Run the Store contract verifier, targeted Store contract test, strict Sphinx
  build, and `git diff --check`.

## Adversarial review

After saving, look for truncated or merged feature entries, accidental uploads,
unsupported claims, stale blocker language, external-state changes outside the
slice, and any wording that could imply Store certification or publication.
Correct every actionable finding and reassess until clean.

## Exit criteria

- All approved non-upload listing text is saved and visibly reconciled.
- No file is selected or uploaded and no certification/publication action is
  taken.
- The remaining screenshot and package gates are explicit.
- Repository evidence and tests agree with the observed Partner Center state.
