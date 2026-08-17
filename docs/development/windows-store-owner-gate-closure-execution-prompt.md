# Microsoft Store owner-gate closure execution prompt

## Objective

Resolve the three remaining Microsoft Store owner-decision gates with exact,
reviewable values before any package selection or upload.

## Verified context

- Partner Center `Submission 1` remains in draft with Pricing and availability,
  Properties, Age ratings, and Submission options complete.
- The manual publication hold is saved.
- Packages and Store listings remain not started; no package or asset has been
  uploaded.
- Five owner attestations are complete. Support/listing disposition,
  device-family/future-family targeting, and exact-candidate binding remain
  pending.

## Scope

1. Inspect the clean repository state, public project URLs, the owner-decision
   packet, and retained exact-candidate evidence.
2. Prepare exact recommendations for the three remaining attestations.
3. Verify live Partner Center state read-only when necessary, without selecting
   or saving device-family controls.
4. Add a single explicit approval statement that the owner can accept or amend.
5. Keep all three attestations pending until the owner supplies that approval.

## Required recommendations

- Website: `https://github.com/lbussy/SDR-Calibration`
- Support URL: `https://github.com/lbussy/SDR-Calibration/issues`
- Privacy-policy field: omit unless Partner Center later requires one.
- Category: `Utilities + tools`; no subcategory; English (United States) only.
- Device availability: Windows 10/11 Desktop only; Mobile, Xbox, Team, Mixed
  Reality, and automatic future-device-family availability disabled.
- Upload candidate: the full `Harness-70ff94c` path, revision `957fbeb`, version
  `0.1.1.0`, and SHA-256 `1d982871...b1edc1`; recompute the hash immediately
  before selection and refuse any mismatch.

## Constraints and non-goals

- Do not infer approval from preparation of this prompt.
- Do not edit or save Partner Center fields.
- Do not select, upload, validate, certify, sign, deliver, or publish anything.
- Do not record credentials, email addresses, phone numbers, postal addresses,
  or notification recipients.
- Do not access SDR hardware or perform RF work.

## Validation and adversarial review

Run the Store source-contract verifier, targeted CTest, strict Sphinx build,
and diff checks. Review for inferred approval, wrong or shortened artifact
binding, an unsuitable public support destination, automatic future-family or
publication exposure, sensitive data, and any upload/certification claim.
Correct every actionable finding and reassess until clean.

## Exit criteria

- All remaining decisions have exact proposed values and one unambiguous owner
  approval statement.
- The three attestations remain unchecked pending the owner's reply.
- No Partner Center state changed.
- Applicable checks pass, and attributable repository changes are committed and
  pushed when authorized.
