# Testing strategy

## Test classes

### Unit tests

Unit tests are deterministic and require no SDR, vendor driver, network,
display server, or external service. They cover calibration math, uncertainty,
profile validation, configuration matching, and parsers using reviewed fixtures.

### Integration tests

Integration tests cover boundaries such as SoapySDR, Qt, filesystem behavior,
and WSJT-X files. Tests that require installed optional components are labeled
and selected explicitly.

### Real-device tests

Real-device tests are never part of an ordinary test run. Each test states:

- exact SDR identity and driver;
- antenna or terminated-input arrangement;
- requested frequency, rate, bandwidth, gain, and clock source;
- duration and observation bound;
- expected evidence and acceptance criteria;
- abort method and cleanup.

This application is receive-oriented. Tests must not cause or imply RF
transmission.

## Baseline commands

```shell
cmake --preset default
cmake --build --preset default
ctest --preset default
```

A zero-test result is reported as `no tests`, not as evidence that behavior is
validated.

Documentation validation is:

```shell
sphinx-build -W -b html docs docs/_build/html
```

The strict release and portable-package checks are:

```shell
cmake --preset release
cmake --build --preset release
ctest --preset release
cmake --build build/release --target package-audit
cmake --build build/release --target package-archive-audit
```

`package-audit` performs an isolated staged install, required-content and SPDX
syntax checks, build-path leakage detection, and CLI startup.
`package-archive-audit` additionally creates, extracts, and checks the portable
archive. These checks do not prove
runtime dependency deployment, clean-host installation, binary-license
compliance, native installer behavior, signing, notarization, hardware support,
or calibration accuracy. Those results require retained evidence in the
platform/device matrix and qualification record.

`branding_contract_tests` is a deterministic, hardware-free source contract
check. It verifies the product-facing name, macOS bundle and internal executable
split, Windows installed-app and Start menu shortcut definition, Linux desktop
entry and icon-theme resolution, native icon container representations, source
PNG dimensions, provenance fields, and every SHA-256 in
`assets/icons/icon-manifest.json`. It does not substitute for a native package
run, human visual review, or clean-host qualification.

`version_identity_contract_tests` makes the CMake project version the single
source for production CLI, capture CLI, Qt application, macOS bundle, Windows
MSI, Debian packages, CPack filenames, and configured SPDX identity. It rejects
semantic-version literals in executable entry points and missing target/package
bindings. `product_version_runtime_tests` separately runs `sdrcal --version`
and requires exact agreement without file, network, device, or RF access.

On macOS, the same test checks the opaque blue-corner contract for the modern
1024-pixel artwork, byte identity with the image embedded in the Icon Composer
document, and the CMake/Info.plist bindings for `Assets.car` plus the
`actool`-generated pre-26 ICNS. A native package review remains necessary to
show which rendition the operating system actually selects.

`package-audit` parses the ICNS actually staged in the application, verifies
all ten generated representation types, the eight PNG dimensions, and the two
legacy ARGB headers and minimum payloads. It also checks both icon metadata keys
and inspects `Assets.car` for the named `SDRCalibration` icon group. It does not
substitute for an actual pre-26 host rendering.

The macOS signed-package path is intentionally separate and disables SoapySDR:

```shell
cmake --preset macos-release \
  -DSDRCAL_MACOS_SIGNING_IDENTITY="Developer ID Application" \
  -DSDRCAL_MACOS_NOTARY_PROFILE="<notarytool-profile>"
cmake --build --preset macos-release
ctest --preset macos-release
cmake --build build/macos-release --target package-audit
cmake --build build/macos-release --target macos-dmg
```

The final target refuses a dirty tree, deploys dynamic Qt frameworks, rejects
absolute non-system runtime paths, signs and notarizes the application and DMG,
staples the ticket, runs Gatekeeper assessments, and writes evidence under
`build/macos-release/macos-package/evidence`. A passing run applies only to the
recorded DMG hash and is not clean-host or binary-license qualification.

The Windows signed-package path is Windows-only and also excludes SoapySDR.
Create and locally trust a non-exportable development certificate once:

```powershell
$certificate = .\packaging\windows\manage-self-signed-certificate.ps1 -Action Create
$thumbprint = ($certificate | Select-String '^Thumbprint: ').Line.Split(': ')[1]
```

Then construct the self-signed package:

```text
cmake --preset windows-release \
  -DSDRCAL_WINDOWS_SIGNING_MODE=SELF_SIGNED \
  -DSDRCAL_WINDOWS_CERTIFICATE_THUMBPRINT=<40-hex-thumbprint>
cmake --build --preset windows-release
ctest --preset windows-release
cmake --build build/windows-release --target package-audit
cmake --build build/windows-release --target windows-msi
```

The final target rejects dirty source, missing tools or credentials, unsafe
output placement, deployment/signing failures, mismatched signing inputs,
invalid extracted payloads, and CLI startup failure. Evidence is written below
`build/windows-release/windows-package/evidence`. Administrative extraction
does not substitute for a clean-host installation test. Self-signed evidence
establishes local integrity only; it does not establish public trust,
SmartScreen reputation, or validity after the certificate expires.

Inspect or remove the exact certificate when it is no longer needed:

```powershell
.\packaging\windows\manage-self-signed-certificate.ps1 `
  -Action Status -Thumbprint <40-hex-thumbprint>
.\packaging\windows\manage-self-signed-certificate.ps1 `
  -Action Remove -Thumbprint <40-hex-thumbprint>
```

Future public-trust signing uses
`-DSDRCAL_WINDOWS_SIGNING_MODE=PUBLIC_TRUST` together with the certificate
thumbprint and an HTTPS `SDRCAL_WINDOWS_TIMESTAMP_URL`.

The separate Microsoft Store path constructs an unsigned pre-submission MSIX
from exact non-secret identity values source-bound after product-name
reservation:

```powershell
cmake --preset windows-store-release `
  -DSDRCAL_QT_SOURCE_ARCHIVE='<matching qtbase source archive>' `
  -DSDRCAL_QT_SOURCE_SHA256='<lowercase SHA-256>'
cmake --build --preset windows-store-release
ctest --preset windows-store-release
cmake --build build/windows-store-release --target package-audit
cmake --build build/windows-store-release --target windows-store-msix
```

The target refuses dirty or upstream-unsynchronized source, placeholder
identity, output reuse, unexpected code, missing Qt closure, incomplete license
disposition, or manifest/package validation failure. Package creation and
unpacking do not establish Store certification, Microsoft signing, public
trust, installation, updates, uninstall behavior, or SmartScreen results.
Those require exact Store-delivered and suitable-host evidence.

The Ubuntu production-package path is Ubuntu 24.04 x86_64-only and excludes
SoapySDR:

```shell
cmake --preset ubuntu-release
cmake --build --preset ubuntu-release
ctest --preset ubuntu-release
cmake --build build/ubuntu-release --target package-audit
cmake --build build/ubuntu-release --target ubuntu-deb
```

The final target refuses dirty or mismatched source/platform inputs, stages
under `/usr` without root, derives system dependencies with `dpkg-shlibdeps`,
builds and extracts the DEB, rejects missing payloads, build-path leakage,
RPATH/RUNPATH, wrong-architecture ELF files and unresolved libraries, verifies
CLI startup, and writes evidence below
`build/ubuntu-release/ubuntu-package/evidence`. Extraction and build-host
startup and Phase 14.4 exact dependency disposition do not substitute for
clean-host installation.

The Raspberry Pi recorded-input CLI path is 64-bit Raspberry Pi OS-only and
excludes Qt, the GUI, SoapySDR, device modules, desktop integration, and icons:

```shell
cmake --preset raspberry-pi-cli-release
cmake --build --preset raspberry-pi-cli-release
ctest --preset raspberry-pi-cli-release
cmake --build build/raspberry-pi-cli-release --target package-audit
cmake --build build/raspberry-pi-cli-release --target package-archive-audit
cmake --build build/raspberry-pi-cli-release --target raspberry-pi-deb
```

The preset applies a 128 MiB per-observation declared-input ceiling selected
from native resource measurement on the first Raspberry Pi fixture.
The DEB target rejects a dirty tree, wrong platform or architecture, mismatched
configuration, GUI/Qt/SoapySDR content, conveyed shared libraries, unresolved
dependencies, RPATH/RUNPATH, wrong-architecture ELF files, build-path leakage,
and failed extracted CLI startup. A native build and audited DEB do not establish
clean-host installation, device support, calibration accuracy, or general
Raspberry Pi compatibility.

## Required fixtures

Planned fixtures include:

- synthetic complex-IQ carriers with known offset, drift, noise, and clipping;
- profile examples that pass and fail each contract gate;
- positive and negative WSJT-X sign-convention cases;
- valid, rejected, malformed, and partially understood `fmt.all` lines;
- bounded and unknown-version WSJT-X inputs, source-digest checks, and
  information-loss reports;
- mocked SoapySDR capability and stream failures;
- fake capture sources covering exact reads, short reads, timeouts,
  discontinuities, cancellation, and cleanup failures;
- complete and explicitly incomplete CF32 capture artifacts with manifests;
- atomic-write and interrupted-update cases.

Fixtures must be small, reviewable, redistributable, and free of credentials or
unnecessary personal and hardware identifiers.

The implemented Phase 2 `capture_unit_tests` target exercises the generic
sample-source boundary, bounded recorder, CF32LE encoding, manifest generation,
artifact publication, cancellation, timeout, stream-event, and cleanup paths.
It does not load SoapySDR or establish real-device behavior.

When SoapySDR support is enabled, `soapy_adapter_unit_tests` exercises only an
injected fake API and device facade plus the pure native-return-code translator.
The production wrapper is compiled and linked but never instantiated by normal
tests, so CTest performs no production enumeration, construction, configuration,
or stream operation. These tests establish adapter logic, not installed-module,
RSP1B, or hardware behavior.

The Phase 4 `capture_cli_argument_tests` target remains available when SoapySDR
is disabled and verifies complete, missing, malformed, duplicate, non-finite,
ambiguous, and excessive requests. When SoapySDR is enabled,
`capture_cli_workflow_tests` uses an injected fake API to verify pre-device
failure, preparation failure, requested/effective reporting, bounded recording,
cancellation, artifacts, and cleanup without constructing a real device.

The Phase 11 `application_workflow_unit_tests` target exercises the complete
shared calibration sequence through an injected fake-device boundary. It
covers exact and ambiguous discovery, authenticated references, bounded
acquisition and safe final state, estimator/acceptance rejection, model and
uncertainty gates, evidence privacy, cancellation, reduced assurance, optional
WSJT-X failure, deterministic native profiles, and repeatable evidence. It does
not load SoapySDR, access files or networks, or establish calibration accuracy.

The synthetic end-to-end workflow qualification extends that fixture through
the production profile consumer boundary. It verifies ordered successful
stages, semantic and SHA-256 integrity, profile-to-evidence and
evidence-to-summary digest links, finite model and uncertainty output,
reliability-quotient policy output, in-domain profile evaluation and sign
convention, plus fail-closed tampering and extrapolation. This qualifies only
deterministic software orchestration; it does not establish physical reference
suitability, SDR accuracy, calibration accuracy, or artifact persistence.

The Phase 12 `production_cli_tests` target exercises the product argument and
recorded-input path, strict request and independent trust input, CF32LE digest
verification, stable JSON/exit behavior, shared-workflow success, cancellation,
existing-output refusal, staging cleanup, and finalized artifact publication.
It uses synthetic local files only and performs no device, network, or RF work.

The production CLI tests additionally cover explicit recorded/live schema
discrimination, cross-mode-field rejection, live resource bounds,
disabled-Soapy rejection before staging, injected live-boundary publication,
and recorded-mode regression. The Soapy adapter suite separately exercises
acquisition and cleanup through fake APIs. Neither suite enumerates host
devices or starts a physical stream.

The [RC-WF source-candidate binding](rc-wf-source-candidate-binding.md)
composes these candidate-specific deterministic results with separately
retained physical-fixture evidence. Physical evidence establishes real bounded
acquisition and acceptance behavior; deterministic fixtures establish fitting,
uncertainty, profile, integrity, privacy, and publication behavior. Neither
evidence class silently substitutes for the other, and a rejected physical
observation remains ineligible for fitting.

The Phase 13 `gui_workflow_tests` target verifies exact production-service
argument/exit and live-factory delegation, thread-safe cancellation state,
bounded regular-file review, symbolic-link refusal, and required/optional result
artifacts. The `gui_widget_tests` target runs with Qt's offscreen platform and
verifies initial control state, recorded/live scope labeling, schema-aware live
review, changed-request re-review, read-only surfaces, and accessibility
metadata. Neither test requires a window server, network, SDR, vendor driver,
or RF.
