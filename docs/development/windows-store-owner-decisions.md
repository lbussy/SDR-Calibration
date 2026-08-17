# Microsoft Store owner decision packet

Status: **Owner approved and saved; upload remains separately authorized**

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
fields are reconciled except for the incomplete English listing and package
upload validation.

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
| Subcategory | None | Approved and saved |
| Listing language | English (United States) only for the first submission | Approved and saved |
| Website | `https://github.com/lbussy/SDR-Calibration` | Approved and saved |
| Support channel | `https://github.com/lbussy/SDR-Calibration/issues` | Approved and saved |
| Privacy answer | No | Approved and saved |
| Privacy policy | Omit unless Partner Center requires one | Approved; no URL entered |
| Age rating | All Other App Types; No for every exposed content/interaction question; all-ages/3+ mappings | Approved and saved |
| Publishing schedule | Manual publication hold; no automatic publication | Approved and saved |
| Device families | Windows 10/11 Desktop only; Mobile, Xbox, Team, and Mixed Reality disabled | Approved and saved |
| Future device families | Automatic availability disabled | Approved and saved |
| Intended upload binding | Full path, revision, version, x64 architecture, and SHA-256 below | Approved; immediate pre-selection hash recheck required |
| Certification submission | Separate explicit authorization required after all preflight evidence and visible fields are independently reconciled | Pending |

## Approved final decision

The public repository is the product website, and its public issue tracker is
the support URL. Both are owner-controlled, require no sensitive contact data,
and keep ordinary support separate from the private vulnerability-reporting
channel in `SECURITY.md`. Omit the privacy-policy field unless Partner Center
later requires one; the owner remains responsible for applicable legal duties.

Select only Windows 10/11 Desktop. Leave Mobile, Xbox, Team, and Mixed Reality
unchecked, and uncheck Microsoft's automatic future-device-family option. This
matches the package's sole `Windows.Desktop` target and avoids unqualified
future-platform availability.

The intended upload is exactly:

- path: `C:\Users\lee\SDR-Calibration-Harness-70ff94c\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`;
- source revision: `957fbeb204177c9ba2a1582e936476244b201b9d`;
- package version: `0.1.1.0`, x64; and
- SHA-256: `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.

Recompute the hash immediately before selecting the file and refuse any
mismatch. Four same-named MSIX files exist, so filename alone is not a binding.

The owner approved the following exact statement on 2026-08-17 UTC:

> I approve the GitHub repository as the website and its Issues page as the
> support URL, with no privacy-policy URL unless Partner Center requires one. I
> approve Windows 10/11 Desktop only, with Mobile, Xbox, Team, Mixed Reality,
> and automatic future-device-family availability disabled. I confirm the full
> MSIX path, revision, version, architecture, and SHA-256 above as the intended
> upload, subject to an immediate pre-selection hash recheck. Record me as the
> approving owner using the current UTC date. This does not authorize upload or
> certification submission.

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
- [x] I approve the category, language, website disposition, and the existence
      of an owner-controlled support channel.
- [x] I approve Windows 10/11 Desktop only and the disposition of Microsoft's
      future-device-family option.
- [x] I approve the product-declaration and system-requirement selections after
      reconciling their visible defaults with implemented behavior.
- [x] I reviewed the exact Partner Center privacy wording and approve the
      answer and any required policy disposition.
- [x] I reviewed and answered every exact visible age-rating question based on
      implemented behavior.
- [x] I require a manual publication hold and understand certification success
      must not automatically publish the product.
- [x] I confirm the candidate bindings above still match the intended upload.

Record the approval date and approving owner only when every box is complete:

- Approval date (UTC): **2026-08-17**
- Approving owner: **Lee Bussy**

Do not record an email address, phone number, postal address, account
credential, notification recipient, or verification material in this file.

## Exit criteria

Every owner attestation is complete and the saved website, support URL, and
device-family state match this packet. The incomplete English listing and
package validation must still be reconciled when separately authorized. Package
selection or upload remains a separate authorized slice, and **Submit for
certification** remains a later explicit authorization.
