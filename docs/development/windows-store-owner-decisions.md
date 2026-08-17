# Microsoft Store owner decision packet

Status: **Partially approved and saved; three owner gates pending**

Prepared: 2026-08-17

## Purpose and boundary

This packet records the product owner's explicit approvals and remaining
decisions. Preparing or committing a proposed value is not approval of that
value. The recorded authorizations cover only the saved state described below;
they do not authorize uploading a package or attachment, requesting
certification, or publishing.

The packet is bound to:

- application version `0.1.1`;
- source revision `957fbeb204177c9ba2a1582e936476244b201b9d`; and
- unsigned Store MSIX SHA-256
  `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.

If any binding changes, every decision below returns to pending.

## Visible-form reconciliation gate

The owner authorized creating `Submission 1`, saving the approved non-sensitive
initial fields, answering the complete IARC questionnaire, accepting the IARC
attestation personally, and applying the manual publication hold. Partner
Center still reports the product and submission `In draft`. Exact visible
fields are reconciled except for the incomplete English listing, package upload
validation, and the remaining owner gates below.

## Decision worksheet

`Recommended` describes the prepared submission plan. `Pending` means the owner
has not approved that value in this record.

| Decision | Recommended value or rule | Approval state |
| --- | --- | --- |
| Base price | Free | Approved and saved |
| Markets | All markets Partner Center permits for the account and truthful declarations | Approved and saved |
| Audience | Public | Approved and saved |
| Discoverability | Available and discoverable | Approved and saved |
| Category | Utilities + tools | Approved and saved |
| Subcategory | None unless Partner Center requires one | Pending |
| Listing language | English (United States) only for the first submission | Approved and saved |
| Website | Omit unless the owner selects an owner-controlled product URL | Pending |
| Support channel | Enter an owner-controlled URL or email directly in Partner Center; do not commit it here | Pending |
| Privacy answer | No | Approved and saved |
| Privacy policy | Omit only if the reconciled Partner Center flow and applicable obligations permit it; otherwise provide an owner-controlled truthful policy | Pending |
| Age rating | All Other App Types; No for every exposed content/interaction question; all-ages/3+ mappings | Approved and saved |
| Publishing schedule | Manual publication hold; no automatic publication | Approved and saved |
| Certification submission | Separate explicit authorization required after all preflight evidence and visible fields are independently reconciled | Pending |

## Observed defaults and exact options

These are observations, not approvals:

- Pricing and availability currently defaults to all worldwide markets,
  inclusion of future markets, public audience, discoverable, release as soon
  as possible, and stop acquisition never. Partner Center reports that a price
  must be configured.
- The exact primary category option is `Utilities + tools`; secondary category
  is optional.
- The exact privacy question is: `Does this product access, collect, or
  transmit personal information (data that could be used to identify a
  person)?` Its choices are Yes and No.
- Support information is optional and can include website, support URL, phone,
  and postal-address fields. Sensitive values must be entered directly and not
  committed.
- All device-family boxes are initially unchecked. The available families are
  Windows 10/11 Desktop, Windows 10 Mobile, Windows 10/11 Xbox, Windows 10 Team,
  and Windows 10 Mixed Reality. Future device families are initially enabled.
- The first IARC branch offers `Game`, `Social or Communication`, and `All
  Other App Types`. The direct-ratings-board/physical-media question defaults
  to No. Partner Center states it will share publisher display name and email
  address with IARC.
- The publishing default is automatic after certification. The required safety
  choice is exactly `Don't publish this submission until I select Publish now`.
- Certification notes belong in Additional Testing Info. Its optional
  credentials table must remain empty because the application requires no
  credentials.

## Technical facts available to the owner

- The Store build is offline and recorded-input-only.
- It implements no project telemetry, advertising, account, cloud service, or
  network transport.
- It declares only `runFullTrust`, for the packaged Win32 Qt GUI and
  `sdrcal.exe` execution alias at normal user integrity.
- It does not install SDR drivers, access live SDR hardware, transmit RF,
  request elevation, install services, or modify machine registry state.
- WACK passed for the exact candidate; this does not replace Store
  certification or an owner privacy/legal determination.
- Four exact-candidate screenshots and the deterministic certification fixture
  are retained and privacy-reviewed but have not been uploaded.

## Owner attestation gate

All boxes must be explicitly completed by the owner before the owner-decision
gate in the submission-readiness packet can pass:

- [x] I approve the price, markets, audience, and discoverability selections.
- [ ] I approve the category, language, website disposition, and the existence
      of an owner-controlled support channel entered outside the repository.
- [ ] I approve Windows 10/11 Desktop only and the disposition of Microsoft's
      future-device-family option.
- [x] I approve the product-declaration and system-requirement selections after
      reconciling their visible defaults with implemented behavior.
- [x] I reviewed the exact Partner Center privacy wording and approve the
      answer and any required policy disposition.
- [x] I reviewed and answered every exact visible age-rating question based on
      implemented behavior.
- [x] I require a manual publication hold and understand certification success
      must not automatically publish the product.
- [ ] I confirm the candidate bindings above still match the intended upload.

Record the approval date and approving owner only when every box is complete:

- Approval date (UTC): **Pending**
- Approving owner: **Pending**

Do not record an email address, phone number, postal address, account
credential, notification recipient, or verification material in this file.

## Exit criteria

This packet becomes approved only when every attestation is checked, both final
approval fields are no longer `Pending`, and the candidate bindings remain
exact. The incomplete English listing and package validation must also be
reconciled when separately authorized. Remaining Partner Center field entry and
upload remain separate authorized slices, and **Submit for certification**
remains a later explicit authorization.
