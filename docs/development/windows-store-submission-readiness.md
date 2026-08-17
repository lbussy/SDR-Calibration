# Microsoft Store submission readiness

Status: Prepared offline; Partner Center remains draft

Prepared: 2026-08-17

## Boundary

This packet prepares the first Microsoft Store MSIX submission without creating
or editing a Partner Center submission, uploading a package, requesting
certification, or publishing the product. Decision 0025 makes Store MSIX the
required Windows release artifact; the self-signed MSI remains testing-only.
No package has been uploaded as part of this preparation.

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

These are proposed values for owner review, not Partner Center state.

| Section | Field | Proposed value or rule | State |
| --- | --- | --- | --- |
| Pricing | Base price | Free | Owner approval required |
| Availability | Markets | All markets permitted by the account and product declarations | Owner approval required |
| Availability | Audience | Public | Owner approval required |
| Availability | Discoverability | Available and discoverable | Owner approval required |
| Availability | Schedule | No automatic publication; hold until the owner selects Publish now | Recommended safety gate |
| Properties | Category | Utilities & tools | Owner/visible-option confirmation required |
| Properties | Subcategory | None unless Partner Center requires one | Ready |
| Properties | Xbox | Not supported | Ready |
| Properties | Website | Omit unless an owner-controlled product page is selected | Owner input optional |
| Properties | Support | Owner-controlled URL or email | Owner input recommended; do not invent |
| Listing | Language | English (United States) only for the first submission | Owner approval required |
| Listing | Product name | `SDR Calibration` | Ready |
| Listing | What's new | Blank for the first submission | Ready |
| Packages | Device families | Windows Desktop only; exclude Xbox, Holographic, and Team | Ready, verify after upload |
| Submission options | Publishing hold | Manual publication | Owner approval required |
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

Use this after replacing the retained evidence locator with the exact
Windows-generated fixture location:

> SDR Calibration is an offline, recorded-input-only Windows desktop utility.
> It requires no login, service, network connection, SDR hardware, driver, or
> elevated operation. To exercise the workflow, use the supplied certification
> fixture set and select its recorded request JSON, separate registry-trust JSON,
> and a new output directory. Review Request shows the bounded input; Start
> Calibration produces profile.json, evidence.json, and summary.json. The CLI is
> exposed through the sdrcal.exe app execution alias. The package intentionally
> rejects live-acquisition requests and existing output directories. Candidate
> version: `0.1.1`. Fixture/evidence instructions: `TBD-BLOCKING`.

The fixture generator is `scripts/prepare-store-certification-fixture.py`. It
derives the same synthetic inputs exercised by `production_cli_tests`, binds
the request to `0.1.1`, validates it through `sdrcal`, and writes hashes plus a
manifest. By default it refuses a dirty or upstream-unsynchronized source
tree. `--development-tree` exists only for pre-commit validation and marks its
output not certification-ready. The retained Windows fixture must be separately
reviewed before upload and must contain no personal, device-secret, credential,
private-key, or sensitive raw-IQ material.

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

No genuine Store screenshot is currently retained. At least one is required;
four desktop screenshots are recommended. Capture them from the exact frozen
Store candidate on Windows 11 with no personal paths, identifiers, raw-IQ data,
credentials, or unqualified result claims visible:

1. clean initial application window;
2. reviewed synthetic recorded-input request;
3. successful result tabs showing non-sensitive synthetic output; and
4. CLI `--help` or `--version` beside the packaged GUI, clearly identified as
   the execution alias.

Do not use a live-SDR screen, hardware photograph, unsupported-device badge,
accuracy claim, certification badge, Microsoft logo, or image from a different
build. Record image dimensions, SHA-256, capture host, scaling, candidate
revision/version, and privacy review. One screenshot is the submission minimum,
but the four-shot set is the readiness exit.

## Fail-closed preflight

- [ ] Exact frozen revision and version exceed the prior development candidate.
- [ ] Clean synchronized native Windows build and all hardware-free tests pass.
- [ ] Package identity, publisher, architecture, version, and sole capability
      match this packet.
- [ ] MakeAppx, unpack, payload, runtime, license, corresponding-source, and
      hash evidence pass for the exact upload.
- [ ] Windows App Certification Kit result is retained or an explicit reason
      for omission is reviewed.
- [ ] Certification fixture is deterministic, synthetic, non-sensitive, and
      independently reviewed.
- [ ] Four exact-candidate screenshots and any optional logos pass privacy and
      claim review.
- [ ] Owner approves price, markets, audience, discoverability, category,
      language, privacy answer/policy, support channel, and age-rating answers.
- [ ] Listing copy, certification notes, and `runFullTrust` justification match
      the exact package behavior.
- [ ] Publishing is held for manual owner action.
- [ ] Partner Center package validation passes.
- [ ] Independent reviewer reconciles every visible Partner Center section with
      this packet before **Submit for certification**.

## Current blockers and external gates

1. Read-only inspection of the current Partner Center draft remains incomplete
   because the Chrome control channel was unavailable; no UI state is inferred.
2. Version `0.1.1` has a clean synchronized Windows package construction at
   revision `2e678b8` and SHA-256
   `e359b10ee29216e733d75544cbe2c6c729c9bee646f17f42d71583dbe9a46168`;
   WACK and the exact-hash local lifecycle remain open.
3. A clean synchronized Windows fixture exists on `ecm-mule`, but four
   exact-candidate screenshots do not exist.
4. Owner selections for commercial availability, category, privacy, support,
   age rating, and manual publishing hold are not recorded.
5. Package upload, certification submission, Microsoft signing, Store delivery,
   clean-host Store lifecycle, and MSI migration/coexistence remain open and
   separately authorized.

No checkbox in this packet changes Partner Center state.
