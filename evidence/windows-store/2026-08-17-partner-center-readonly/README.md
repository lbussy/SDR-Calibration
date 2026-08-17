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

The Packages page still contained no uploaded package. Windows 10/11 Desktop,
Mobile, Xbox, Team, and Mixed Reality were all unchecked. `Let Microsoft decide
whether to make this app available to any future device families` remained
checked. No device-family value was changed or saved.
