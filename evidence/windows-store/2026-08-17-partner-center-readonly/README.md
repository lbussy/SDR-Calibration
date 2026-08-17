# Partner Center draft read-only inspection

Date: 2026-08-17

## Result

A read-only inspection of the signed-in Partner Center Apps and games pages
confirmed the current visible state of the reserved product:

- the Apps and games overview listed exactly one product, `SDR Calibration`;
- its product type was `MSIX or PWA app`;
- the product-list status was `Not started`;
- the application overview status was `In draft` and offered `Start
  submission`, establishing that no submission had been started;
- Current packages contained no package entries;
- Manage app names showed `SDR Calibration` as `Reserved for this app`;
- the visible package identity name, publisher, publisher display name, package
  family name, and Store ID matched the committed reservation record exactly;
  and
- Store and Web Store links remained unavailable until the product is live.

No submission was started, package uploaded, field edited, name reserved or
released, attestation made, agreement accepted, validation requested, or
publication action taken. The inspection did not retain screenshots because
the surrounding account interface displayed user/account context not needed
for this evidence. No account, tenant, authentication, contact, tax, payout, or
other sensitive information was copied into the repository.

A follow-up read-only inspection after the owner-decision packet was prepared
reconfirmed product type `MSIX or PWA app`, overview status `In draft`, and the
single `Start submission` action. No pricing, availability, properties,
age-rating, listing, package, or submission-options forms were visible before
that action. Because `Start submission` creates Partner Center submission
state, it was not selected during the read-only slice. Exact visible field
wording and options therefore remain unreconciled pending separate
authorization to create the submission draft.

## Authorized submission-draft inspection

The owner subsequently authorized selecting `Start submission` for the bounded
purpose of creating and inspecting the draft without entering or saving field
values. Partner Center created `Submission 1`, dated 2026-08-17. The overview
showed Pricing and availability, Properties, Age ratings, Packages, and Store
listings as `Not started`; Submission options was `Recommended`; and `Submit
for certification` was disabled.

Read-only section inspection established:

- Pricing requires an explicit price. Defaults were all worldwide markets,
  future markets included, public audience, discoverable, release as soon as
  possible, and stop acquisition never.
- Properties offered the exact primary category `Utilities + tools`, an
  optional secondary category, the personal-information Yes/No question,
  optional website/support/contact/address fields, product declarations, and
  system requirements.
- Age ratings defaulted to a new IARC questionnaire and exposed `Game`, `Social
  or Communication`, and `All Other App Types`, plus a physical-media/direct
  ratings-board Yes/No question. Further questions require selecting an app
  type, so they were not exposed in this read-only slice. The page states that
  publisher display name and email address will be shared with IARC.
- Packages accepted MSIX-family formats. All five device-family boxes were
  unchecked; `Let Microsoft decide whether to make this app available to any
  future device families` was checked.
- Store listings had no package-supported languages before upload. The
  additional-language dialog offered 831 locale choices and had none selected;
  it was closed with Cancel.
- Submission options defaulted to publishing as soon as certification passes.
  The alternative exact choice was `Don't publish this submission until I
  select Publish now`.
- Additional Testing Info provided one certification-description field and an
  optional credentials table; both were empty and no credentials were added.

No form value was entered or selected, no Save action was used, and no package,
fixture, screenshot, or listing was uploaded. No agreement or attestation was
accepted, no validation was requested, and no certification or publication
action occurred.

## Boundaries

This record establishes only the visible Partner Center draft and field state
at inspection time. It does not establish listing completeness, package
validation, certification, Microsoft signing, Store availability, delivery,
update behavior, sales configuration, payment or tax readiness, or publication.

## Owner-authorized saved draft state

The owner subsequently approved and saved these non-sensitive values:

- Pricing and availability: free, all worldwide and future markets, public,
  discoverable, release as soon as possible, and stop acquisition never.
- Properties: `Utilities + tools`, no secondary category, personal-information
  answer No, no support fields, no immersive display mode, every product
  declaration unchecked, and all hardware requirements unspecified.
- Age ratings: `All Other App Types`, No for all nine exposed content and
  interaction questions, No for direct-board/physical-media ratings, and the
  owner personally accepted the IARC Terms of Use and age-of-majority
  attestation. The saved ratings are ESRB Everyone, PEGI 3+, IARC 3+, Microsoft
  3+, and corresponding all-ages mappings; the Current Rating ID was Pending.
- Listing language: English (United States) was added as an additional Store
  listing language. Its required description and screenshot fields were
  exposed but not populated.
- Submission options: `Don't publish this submission until I select Publish
  now` was selected and saved.

Partner Center reported Pricing and availability, Properties, Age ratings, and
Submission options `Complete`; Packages and Store listings remained `Not
started`; and `Submit for certification` remained disabled.

## Exact pre-upload inspection

The intended upload remains on `ecm-mule` at
`C:\Users\lee\SDR-Calibration-Harness-70ff94c\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`.
It was rehashed as
`1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`.
Its embedded evidence binds source revision
`957fbeb204177c9ba2a1582e936476244b201b9d`, identity
`LeeBussy.SDRCalibration`, publisher
`CN=66465467-9B9D-4BDE-9CC9-BE392698D910`, version `0.1.1.0`, x64, and only
`runFullTrust`. The unpacked manifest targets only `Windows.Desktop` with
minimum version `10.0.22000.0`.

Four same-named `0.1.1` MSIX files were found under `C:\Users\lee`. Their
hashes differ. A later upload must use the full `Harness-70ff94c` path above and
must recompute the SHA-256 immediately before selection; filename and version
alone are insufficient. Refuse the upload if either binding differs.

## Owner-approved final gate reconciliation

The owner approved the repository website, Issues support URL, Desktop-only
device availability, disabled automatic future-device-family availability, and
the exact candidate binding on 2026-08-17 UTC. The approved non-sensitive URLs
were saved in Properties:

- website: `https://github.com/lbussy/SDR-Calibration`;
- support: `https://github.com/lbussy/SDR-Calibration/issues`; and
- privacy-policy URL: omitted because the current flow did not require one.

On Packages, Windows 10/11 Desktop was selected and saved. Mobile, Xbox, Team,
and Mixed Reality remained unchecked, and `Let Microsoft decide whether to make
this app available to any future device families` was unchecked and saved. No
file picker was used and no package was selected or uploaded. Partner Center
then reported Packages `Incomplete`, Store listings `Not started`, and **Submit
for certification** disabled. No certification or publication action occurred.

## Owner-approved English listing text

The approved non-upload English (United States) listing fields were entered and
saved from the committed readiness packet:

- the exact description and short description;
- eight separate product-feature entries;
- five keywords: `SDR`, `frequency calibration`, `IQ analysis`, `radio`, and
  `WSJT-X`;
- `Copyright (c) 2026 Lee Bussy`; and
- developer `Lee Bussy`.

What's new remained blank for the first submission. Short title, voice title,
additional license terms, Xbox-only fields, trailers, logos, and optional image
fields remained blank. The saved listing was re-opened and the description,
short description, eight feature entries, five keyword chips, copyright, and
developer values were visibly present. Desktop screenshots remained at zero,
so the listing remained `Incomplete`; Packages also remained `Incomplete` and
**Submit for certification** remained disabled. No file picker was opened and
no screenshot, image, trailer, package, fixture, or other attachment was
selected or uploaded.

## Exact-candidate Desktop screenshot upload

The owner-authorized screenshot slice revalidated the four retained PNG hashes
and dimensions against their manifest, then uploaded exactly those files to the
Desktop English (United States) listing in manifest order. The persisted count
was Desktop four and Xbox zero. The following factual captions were saved in
the same order:

1. `SDR Calibration desktop application ready for a recorded-input workflow.`
2. `Reviewed synthetic calibration request with explicit device and reference provenance.`
3. `Successful synthetic calibration result with reviewable profile, evidence, and summary output.`
4. `Packaged Windows GUI alongside the sdrcal.exe command-line version output.`

No logo, trailer, promotional image, Xbox asset, package, certification fixture,
or other file was selected. After save and reload, Partner Center reported
Store listings `Complete`, Packages `Incomplete`, and **Submit for
certification** disabled. No certification or publication action occurred.

## First package acceptance result

Immediately before transfer, the approved Windows path was rehashed as
`1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1` with
byte size `95910373`. The isolated transfer copy matched both values and was
selected as the only package. Partner Center rejected it with acceptance errors
reporting the three manifest-referenced logo paths as not found, a blank
publisher display name instead of `Lee Bussy`, no supported language, and an
empty unsupported default language. It separately warned that `runFullTrust`
requires approval, as expected.

The package retained on the Packages page is faulty and cannot be saved.
Desktop-only and future-family selections remain unchanged, Packages remains
`Incomplete`, and **Submit for certification** remains disabled. No fixture,
certification, or publication action occurred. Source remediation now declares
the exact `en-us` resource language and strengthens post-pack checks for exact
asset paths and publisher display name. A replacement package will have a new
revision and hash and therefore requires complete qualification and renewed
owner binding before upload.

The owner subsequently authorized deletion of that single rejected package.
Partner Center removed it without presenting a second confirmation dialog. The
Packages page then contained no package entry; Windows 10/11 Desktop remained
checked, Mobile, Xbox, Team, Mixed Reality, and automatic future-family
availability remained unchecked, and Save required no action. No replacement,
fixture, certification, or publication action occurred. The rejected draft
package cannot be recovered from Partner Center; the original qualified bytes
remain on `ecm-mule` at the recorded source path.
