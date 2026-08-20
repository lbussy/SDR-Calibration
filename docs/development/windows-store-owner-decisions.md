# Microsoft Store owner decision packet

Status: **Submitted for certification; publication not authorized**

Prepared: 2026-08-17

Updated: 2026-08-20

## Purpose and boundary

This packet records the product owner's explicit approvals and remaining
decisions. Preparing or committing a proposed value is not approval of that
value. The recorded authorizations cover only the saved state and exact
replacement package and certification submission described below; they do not
authorize uploading any other attachment or publishing.

The current approved replacement binding is:

- application version `0.1.1`;
- source revision `4406a82e01072afc0d61d2516c2fe9607c608ea4`; and
- unsigned Store MSIX SHA-256
  `6d6998bb2130b9f137ac2847c8449f24259f5a526f1f8c67d66f7953f9327f08`.

The prior binding at revision `957fbeb` and SHA-256 `1d982871...` was
owner-authorized for upload and subsequently deleted from the draft after
Partner Center rejected it. Saved commercial, listing, property, rating,
publication-hold, and device-family decisions remain approved. On 2026-08-20
UTC, the owner explicitly approved the complete replacement path, revision,
version, architecture, size, and hash binding recorded below, then separately
authorized selecting and uploading that exact package. The replacement was
accepted and saved; certification was not submitted.

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
| Secondary category | Developer tools | Approved and saved after Partner Center would not persist clearing the optional value |
| Subcategory | None | Approved and saved |
| Listing language | English (United States) only for the first submission | Approved and saved |
| Website | `https://github.com/lbussy/SDR-Calibration` | Approved and saved |
| Support channel | `https://github.com/lbussy/SDR-Calibration/issues` | Approved and saved |
| Privacy answer | Yes; the app accesses user-selected local files that may contain personal information, without project collection or transmission | Package-derived selection reconciled and saved |
| Privacy policy | `https://github.com/lbussy/SDR-Calibration/blob/main/PRIVACY.md` | Approved, publicly reachable, and saved |
| Age rating | All Other App Types; No for every exposed content/interaction question; all-ages/3+ mappings | Approved and saved |
| Publishing schedule | Manual publication hold; no automatic publication | Approved and saved |
| Device families | Windows 10/11 Desktop only; Mobile, Xbox, Team, and Mixed Reality disabled | Approved and saved |
| Future device families | Automatic availability disabled | Approved and saved |
| Intended upload binding | Replacement path, revision, version, x64 architecture, size, and SHA-256 below | Approved 2026-08-20 UTC; immediate pre-selection hash recheck required |
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

The previously approved and now rejected upload was exactly:

- path: `C:\Users\lee\SDR-Calibration-Harness-70ff94c\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`;
- source revision: `957fbeb204177c9ba2a1582e936476244b201b9d`;
- package version: `0.1.1.0`, x64; and
- SHA-256: `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.

Recompute the hash immediately before selecting the file and refuse any
mismatch. Four same-named MSIX files exist, so filename alone is not a binding.

The owner-approved qualified replacement binding is exactly:

- path: `C:\Users\lee\SDR-Calibration-Store-0.1.1-h-4406a82\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`;
- source revision: `4406a82e01072afc0d61d2516c2fe9607c608ea4`;
- package version: `0.1.1.0`, x64;
- byte size: `95910402`; and
- SHA-256: `6d6998bb2130b9f137ac2847c8449f24259f5a526f1f8c67d66f7953f9327f08`.

This replacement passed construction, unpacked manifest/logo inspection, all
19 hardware-free tests, package audit, and the same-host development lifecycle.
It has not been selected or uploaded, and the local results do not establish
Partner Center acceptance. The binding is approved, but any file selection or
upload remains a separate authorized action.

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

All saved-field and replacement-binding boxes are explicitly complete:

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
- [x] I approve the complete replacement candidate binding above as the
      intended upload, subject to an immediate pre-selection hash recheck.

The original saved-field and rejected-candidate approval remains recorded as:

- Approval date (UTC): **2026-08-17**
- Approving owner: **Lee Bussy**

The replacement binding approval is recorded as:

- Replacement approval date (UTC): **2026-08-20**
- Approving owner: **Lee Bussy**

The separate replacement upload authorization is recorded as:

> I authorize selecting and uploading the exact approved replacement package.

- Upload authorization date (UTC): **2026-08-20**
- Approving owner: **Lee Bussy**
- Scope: the exact approved replacement only; no certification submission or
  publication.

After Partner Center required a privacy policy for the accepted package, the
owner authorized creation of a repository-hosted policy on 2026-08-20 UTC. Its
intended Partner Center URL is:

`https://github.com/lbussy/SDR-Calibration/blob/main/PRIVACY.md`

Creating and publishing the policy does not authorize entering or saving the
URL in Partner Center or submitting the product for certification.

The owner subsequently entered and saved the exact privacy-policy URL and the
478-character `runFullTrust` justification. After two attempts to clear the
optional secondary category did not persist, the owner explicitly approved
retaining **Developer tools** on 2026-08-20 UTC. All submission sections then
reported `Complete`; **Submit for certification** was enabled but not selected.

Do not record an email address, phone number, postal address, account
credential, notification recipient, or verification material in this file.

## Certification submission gate

The final read-only audit found all six sections `Complete`, the sole package
`Validated`, the manual publication hold selected, and no required supplemental
testing field. Certification submission remains a separate external action.
The exact authorization statement for a later slice is:

> I authorize submitting Partner Center Submission 1, internal ID
> 1152921505701677071, for Microsoft Store certification with the currently
> saved package, listing, properties, ratings, targeting, runFullTrust
> justification, and manual publication hold. Do not publish the product.

Do not treat preparation or quotation of that statement as authorization.

The owner supplied that exact authorization on 2026-08-20 UTC. Partner Center
accepted Submission 1 for certification and reported:

- product status `In certification`;
- Submission `Complete`;
- Pre-processing `In progress`;
- Certification `Not started`; and
- Publishing `Not started`.

Partner Center also confirmed: `Your product will start publishing when you
click on Publish now.` No publication authorization was given, and **Publish
now** was not selected.

## Exit criteria

The saved website, support URL, device-family state, replacement binding,
completed replacement upload, and certification submission match this packet.
Publication remains a later explicit authorization.
