# Microsoft Store submission readiness

Status: Replacement package accepted and saved; certification not submitted

Prepared: 2026-08-17

Updated: 2026-08-20

## Boundary

This packet began as preparation for the first Microsoft Store MSIX submission
and now records the owner-authorized draft edits, listing assets, and accepted
replacement package. Decision 0025 makes Store MSIX the required Windows
release artifact; the self-signed MSI remains testing-only. Certification
submission and publication remain separately authorized.

The submission must use these established values:

| Field | Exact value |
| --- | --- |
| Product name | `SDR Calibration` |
| Store ID | `9PK9W5S2D66L` |
| Package identity | `LeeBussy.SDRCalibration` |
| Publisher | `CN=66465467-9B9D-4BDE-9CC9-BE392698D910` |
| Publisher display name | `Lee Bussy` |
| Architecture/device family | x64, Windows Desktop |
| Minimum Windows version | Windows 11, `10.0.22000.0` |
| Restricted capabilities | `runFullTrust` only |

Account credentials, contact information, addresses, notification recipients,
and verification materials must not enter the repository.

## Official submission sections

Microsoft's current MSIX checklist requires pricing/availability, properties,
answers to every age-rating question, at least one package, and at least one
language listing with a description and screenshot. Restricted-capability
justification is required when a package declares one. Sources:

- [Create an MSIX app submission](https://learn.microsoft.com/windows/apps/publish/publish-your-app/msix/create-app-submission)
- [Add Store listing information](https://learn.microsoft.com/windows/apps/publish/publish-your-app/msix/add-and-edit-store-listing-info)
- [Privacy policy and support information](https://learn.microsoft.com/windows/apps/publish/publish-your-app/msix/support-info)
- [Manage submission options](https://learn.microsoft.com/windows/apps/publish/publish-your-app/msix/manage-submission-options)
- [Microsoft Store policies](https://learn.microsoft.com/windows/apps/publish/store-policies)

Requirements can change. Recheck these official pages and the visible Partner
Center form immediately before entering or approving submission data.

## Recommended field plan

These values are the approved submission plan. Saved Partner Center state is
identified explicitly below. The approval record is the
[Microsoft Store owner decision packet](windows-store-owner-decisions.md).

| Section | Field | Proposed value or rule | State |
| --- | --- | --- | --- |
| Pricing | Base price | Free | Approved and saved |
| Availability | Markets | All markets permitted by the account and product declarations | Approved and saved |
| Availability | Audience | Public | Approved and saved |
| Availability | Discoverability | Available and discoverable | Approved and saved |
| Availability | Schedule | No automatic publication; hold until the owner selects Publish now | Approved and saved |
| Properties | Category | Utilities + tools | Approved and saved |
| Properties | Subcategory | None | Approved and saved |
| Properties | Xbox | Not supported | Approved and saved |
| Properties | Website | `https://github.com/lbussy/SDR-Calibration` | Approved and saved |
| Properties | Support | `https://github.com/lbussy/SDR-Calibration/issues` | Approved and saved |
| Properties | Privacy policy | `https://github.com/lbussy/SDR-Calibration/blob/main/PRIVACY.md` | Repository-hosted policy created; Partner Center entry pending |
| Listing | Language | English (United States) only for the first submission | Approved and saved |
| Listing | Product name | `SDR Calibration` | Approved and saved |
| Listing | Description | Exact English listing copy below | Approved and saved |
| Listing | Product features | Eight separate features below | Approved and saved |
| Listing | What's new | Blank for the first submission | Approved and saved |
| Listing | Short description | Exact short description below | Approved and saved |
| Listing | Keywords | `SDR`, `frequency calibration`, `IQ analysis`, `radio`, `WSJT-X` | Approved and saved |
| Listing | Copyright | `Copyright (c) 2026 Lee Bussy` | Saved; matches repository license |
| Listing | Developed by | `Lee Bussy` | Saved |
| Packages | Device families | Windows Desktop only; exclude Mobile, Xbox, Team, and Mixed Reality | Approved, reverified after replacement upload, and saved |
| Submission options | Publishing hold | Manual publication | Approved and saved |
| Submission options | Certification notes | Use the text below | Ready after candidate binding |
| Submission options | Restricted capability | Use the `runFullTrust` justification below | Ready after candidate binding |

## English listing copy

### Short description

Create traceable SDR frequency-calibration profiles from bounded recorded
complex-IQ observations, with explicit uncertainty, provenance, device binding,
and reviewable result files.

### Description

SDR Calibration analyzes recorded complex-IQ observations and creates a native
frequency-calibration profile for one explicitly identified SDR configuration.
The workflow keeps device identity, requested and effective settings, reference
provenance, observation acceptance, model validity, uncertainty, and evidence
assurance independently reviewable.

Choose a versioned recorded-calibration request, its separate local registry
trust file, and a new output directory. Review the request before starting.
Successful runs publish a native profile together with evidence and summary
files; an optional WSJT-X projection is clearly identified as lossy and never
replaces the native profile.

This Store build is offline and recorded-input-only. It does not discover or
control an SDR, include SDR drivers or vendor modules, stream samples from
hardware, transmit RF, modify an installed WSJT-X configuration, activate a
profile automatically, or claim that any receiver, reference, or calibration
result is qualified. Users supply their own compatible request, observation,
and trust-pin files. Scientific acceptance remains bounded by the evidence and
uncertainty recorded in each result.

The package includes a graphical Windows application and the `sdrcal.exe`
command-line execution alias. Project source and documentation are MIT
licensed; bundled Qt libraries retain their applicable licenses and
corresponding-source materials.

### Product features

Enter these as separate features; Partner Center renders its own bullets.

1. Recorded complex-IQ calibration workflow
2. Native device-bound calibration profiles
3. Explicit uncertainty and validity domain
4. Reviewable provenance and evidence assurance
5. Atomic profile, evidence, and summary output
6. Optional lossy WSJT-X projection
7. Desktop GUI and command-line execution alias
8. Offline Store build with no bundled SDR drivers

### Search terms

Use only if Partner Center presents the optional field:

`SDR`, `frequency calibration`, `IQ analysis`, `radio`, `WSJT-X`

## Certification notes

Use this with the retained supplemental fixture attachment:

> SDR Calibration is an offline, recorded-input-only Windows desktop utility.
> It requires no login, service, network connection, SDR hardware, driver, or
> elevated operation. To exercise the workflow, use the supplied certification
> fixture set and select its recorded request JSON, separate registry-trust JSON,
> and a new output directory. Review Request shows the bounded input; Start
> Calibration produces profile.json, evidence.json, and summary.json. The CLI is
> exposed through the sdrcal.exe app execution alias. The package intentionally
> rejects live-acquisition requests and existing output directories. Candidate
> version: `0.1.1`. Attach
> `SDRCalibration-0.1.1-Store-Certification-Fixture.zip` (SHA-256
> `f11547cdbbedba715074d55413026a86ac5209597aeb42f8712e7527c7b6ff51`),
> extract it, select `request.json` and `trust.json`, and choose any new output
> directory. The archive README and `SHA256SUMS` provide bounded verification
> instructions.

The fixture generator is `scripts/prepare-store-certification-fixture.py`. It
derives the same synthetic inputs exercised by `production_cli_tests`, binds
the request to `0.1.1`, validates it through `sdrcal`, and writes hashes plus a
manifest. By default it refuses a dirty or upstream-unsynchronized source
tree. `--development-tree` exists only for pre-commit validation and marks its
output not certification-ready. The retained Windows fixture must be separately
reviewed before upload and must contain no personal, device-secret, credential,
private-key, or sensitive raw-IQ material.

That review is retained with the proposed supplemental attachment at
`evidence/windows-store/2026-08-17-0.1.1-certification-fixture/`. The attachment
has not been uploaded; its filename and hash must be rechecked immediately
before it is added to a Partner Center submission.

## `runFullTrust` justification

> SDR Calibration is a C++20/Qt 6 packaged desktop application using the
> Windows.FullTrustApplication entry point. `runFullTrust` is required to launch
> the Win32 Qt GUI and the packaged sdrcal.exe command-line execution alias at
> normal user integrity. The application reads only files explicitly selected
> or named by the user and writes only to a new user-selected output directory.
> It does not request elevation, install services or drivers, modify machine
> registry state, access SDR hardware in this Store build, or declare any other
> restricted capability.

## Privacy assessment

Source and package inspection support a preliminary **No** answer to the
Partner Center question asking whether the Store build accesses, collects, or
transmits personal information:

- the Store artifact is recorded-input-only and contains no SoapySDR/vendor
  modules or live-device path;
- the workflow processes user-supplied local files and writes local results;
- no project telemetry, advertising, account, cloud service, or network
  transport is implemented; and
- `Qt6Network.dll` in the deployed closure is a Qt deployment dependency, not
  evidence that the application transmits data.

This is a technical assessment, not legal advice. Before submission, the owner
must confirm the exact frozen candidate, Partner Center's capability-derived
privacy determination, applicable markets/law, and whether support or crash
reporting introduces any collection. If Partner Center marks the answer Yes or
the candidate collects/transmits personal information, provide truthful policy
text or an owner-controlled policy URL. Never use Microsoft's privacy policy as
the application's policy.

## Age-rating preparation

The owner must attest to every visible questionnaire answer. Based on current
implemented Store scope, the expected answers are no violence, sexual content,
controlled substances, gambling, user-generated content, unrestricted web
access, purchases, location sharing, or online interaction. Reconcile the exact
questionnaire wording at submission time; do not mechanically paste this
summary as answers.

## Listing asset plan

Existing approved source art can support package/store logos:

- `assets/icons/source/sdr-calibration-master-1024.png`
- `assets/icons/source/sdr-calibration-small-master-1024.png`
- `assets/icons/icon-manifest.json`

Four genuine Store screenshot candidates are retained in
`evidence/windows-store/2026-08-17-0.1.1-screenshots/`. They were captured from
the exact frozen Store candidate on Windows 11 and independently reviewed for
personal paths, identifiers, raw-IQ data, credentials, and unqualified result
claims:

1. clean initial application window;
2. reviewed synthetic recorded-input request;
3. successful result tabs showing non-sensitive synthetic output; and
4. CLI `--help` or `--version` beside the packaged GUI, clearly identified as
   the execution alias.

The evidence manifest binds every image to revision
`957fbeb204177c9ba2a1582e936476244b201b9d`, version `0.1.1`, and MSIX SHA-256
`1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`, and
records dimensions, hashes, capture host, and privacy disposition. Do not use a
live-SDR screen, hardware photograph, unsupported-device badge,
accuracy claim, certification badge, Microsoft logo, or image from a different
build. Record image dimensions, SHA-256, capture host, scaling, candidate
revision/version, and privacy review. One screenshot is the submission minimum,
but the four-shot set is the readiness exit.

## Fail-closed preflight

- [x] Exact Store candidate revision and version exceed the prior development candidate.
- [x] Clean synchronized native Windows build and all hardware-free tests pass.
- [x] Package identity, publisher, architecture, version, and sole capability
      match this packet.
- [x] MakeAppx, unpack, payload, runtime, license, corresponding-source, and
      hash evidence pass for the exact upload.
- [x] Windows App Certification Kit result is retained or an explicit reason
      for omission is reviewed.
- [x] Certification fixture is deterministic, synthetic, non-sensitive, and
      independently reviewed.
- [x] Four exact-candidate screenshots pass privacy and
      claim review.
- [x] Four exact-candidate Desktop screenshots and factual captions are saved
      in Partner Center in manifest order; Xbox and optional assets remain empty.
- [x] Owner completed the support-channel, device-family/future-family, and
      exact-candidate binding attestations.
- [x] Listing copy, certification notes, and `runFullTrust` justification match
      the exact package behavior.
- [x] Publishing is held for manual owner action.
- [ ] Partner Center package validation passes.
- [ ] Independent reviewer reconciles every visible Partner Center section with
      this packet before **Submit for certification**.

## Current blockers and external gates

1. Partner Center reports Pricing and availability, Properties, Age ratings,
   and Submission options `Complete`. The English (United States) listing
   has all approved text and four exact-candidate Desktop screenshots saved;
   Store listings is `Complete` and Packages is `Incomplete`;
   `Submit for certification` remains disabled.
2. Version `0.1.1` has a clean synchronized Windows package construction at
   revision `957fbeb` and SHA-256
   `1d9828710dcec5c93862e217606a92f53c1470b5abb412a23725ebc811b1edc1`;
   WACK and the exact-hash same-host development lifecycle passed. This does
   not establish Microsoft signing, Store delivery, or clean-host behavior.
3. Four exact-candidate screenshots derived from the clean synchronized fixture
   on `ecm-mule` are retained with hashes, dimensions, and privacy review. They
   are saved to the Desktop English (United States) listing in manifest order
   with factual captions; Xbox and optional asset counts remain zero.
4. The deterministic certification fixture attachment is retained and
   independently reviewed, but it has not been uploaded.
5. The owner approved and saved commercial availability, category, privacy,
   reconciled Properties declarations, the repository website and Issues
   support URL, all IARC answers and attestation, the manual publication hold,
   and Desktop-only device availability with automatic future-family
   availability disabled. The exact-candidate binding is owner-approved subject
   to an immediate pre-selection hash recheck.
6. The exact intended upload was rehashed on `ecm-mule` at
   `C:\Users\lee\SDR-Calibration-Harness-70ff94c\build\windows-store-release\windows-store-package\SDRCalibration-0.1.1-Windows-Store-x64.msix`;
   its SHA-256, embedded revision, identity, Desktop target, and sole
   `runFullTrust` capability match this packet. Four same-named MSIX files exist
   on the host, so a later upload must bind the full path and recomputed hash,
   not the filename alone.
7. The Packages page has Windows 10/11 Desktop checked; Mobile, Xbox, Team, and
   Mixed Reality unchecked; and Microsoft's future-device-family option
   unchecked. The rejected first package was deleted; no replacement package
   has been selected or uploaded.
8. Package and fixture upload, certification submission, Microsoft signing,
   Store delivery, clean-host Store lifecycle, and MSI migration/coexistence
   remain open and separately authorized.
9. The exact approved `0.1.1` candidate was uploaded after immediate source and
   transfer hash checks, but Partner Center rejected it for missing resolved
   logo paths, blank publisher display name, and absent/default package language.
   The expected `runFullTrust` approval warning was also shown. The owner
   authorized deletion and Partner Center returned to zero package entries with
   the approved device-family state unchanged. Package construction now parses
   the three logo references from the unpacked manifest, requires their exact
   payload paths, opens them as PNG images, and verifies their required square
   dimensions. These checks prevent path or image drift but do not prove the
   Partner Center errors resolved. That proof requires acceptance of a new
   revision after rebuild, full requalification, and renewed owner binding.
10. A fresh replacement at revision `4406a82` and SHA-256
    `6d6998bb2130b9f137ac2847c8449f24259f5a526f1f8c67d66f7953f9327f08`
    passed all 19 hardware-free tests, package audit, MakeAppx pack/unpack,
    exact unpacked language/publisher/logo inspection, development-signed
    install, CLI alias, two GUI launches, uninstall, and cleanup. No Partner
    Center action occurred during that build slice. The owner approved the
    complete replacement binding on 2026-08-20 UTC.
11. After separate explicit authorization, the exact replacement was rehashed
    at its full retained Windows path and after isolated transfer. Both checks
    matched byte size `95910402` and SHA-256
    `6d6998bb2130b9f137ac2847c8449f24259f5a526f1f8c67d66f7953f9327f08`.
    Partner Center Submission 1, internal ID `1152921505701677071`, accepted
    the single package as version `0.1.1.0`, `X64`, for `Windows.Desktop` with
    minimum version `10.0.22000.0`. Desktop remained checked; Mobile, Xbox,
    Team, Mixed Reality, and automatic future-family availability remained
    unchecked. The only visible package-validation result was the expected
    `runFullTrust` approval warning. The package state was saved and the product
    remained `In draft`; certification was not submitted.

Saved-field owner gates, the replacement artifact binding, and the separate
upload authorization are recorded in `windows-store-owner-decisions.md`.
Certification submission and publication remain unauthorized.

Partner Center's package-derived privacy assessment requires a policy despite
the Store build's local-only processing. The owner authorized the
repository-hosted policy at
`https://github.com/lbussy/SDR-Calibration/blob/main/PRIVACY.md`; entering and
saving that URL remains the next separate Partner Center action.

No checkbox in this packet changes Partner Center state.
