# Licensing and Distribution Policy

Status: Adopted project policy

Project source license: MIT

## 1. Project license

Original SDR Calibration source code and documentation are licensed under the
MIT License unless an individual file clearly states otherwise.

The MIT License does not apply to or relicense Qt, SoapySDR, SDR vendor modules,
system libraries, build tools, or any other third-party component.

## 2. Qt usage

The GUI uses Qt 6 Widgets. The open-source build uses only Qt modules whose
licenses are compatible with the project's intended distribution.

Qt is dynamically linked by default. The initial approved module set is:

- Qt Core
- Qt Widgets
- Qt Network
- Qt Concurrent
- Qt OpenGLWidgets when justified by measured display requirements

Adding another Qt module requires a license and transitive-dependency review.
GPL-only modules require an explicit project-level decision before use and must
not enter the build through an incidental dependency.

## 3. Binary distribution gate

A release containing Qt or another third-party component must not be published
until its package includes or provides all materials required by the versions
actually distributed. For LGPL Qt builds, the release process must address at
least:

1. Prominent notice that the application uses Qt.
2. The applicable LGPL and other license texts.
3. Copyright and third-party notices for the included modules.
4. Complete corresponding Qt source, including applied modifications, or a
   controlled written offer and retrieval method that satisfies the license.
5. Instructions and a deployment design that allow replacement of the Qt
   shared libraries with interface-compatible modified versions.
6. Source for modifications made to LGPL-covered Qt components.
7. Confirmation that signing, notarization, installers, application stores,
   and device policies do not prohibit the rights granted by the applicable
   licenses.
8. A dependency inventory for the exact release on every target platform.

Passing application tests does not satisfy this distribution gate.

## 4. Build and CI policy

The build system must keep project targets distinct from third-party targets.
CI should eventually produce a machine-readable software bill of materials and
a release-specific third-party notice bundle.

Dependency versions and licenses must be pinned or otherwise reproducible for
release builds. Packaging checks must fail when a distributed library lacks a
recorded license, notice, or corresponding-source disposition.

## 5. Static linking and appliances

Static Qt linking is outside the default distribution model and requires an
explicit review of relinking, object-file, installation-information, and other
license obligations.

A Raspberry Pi used as a test fixture is treated as a normal Linux test target.
Distributing a locked or preconfigured appliance requires a separate review of
the user's ability to replace and run modified LGPL-covered components.

## 6. Commercial Qt

The project does not require a commercial Qt license under this policy.
Commercial licensing may be evaluated later if distribution requirements,
locked-device constraints, support needs, or desired modules make the
open-source terms unsuitable.

Changing licensing strategy must be deliberate and documented. Open-source and
commercial Qt materials must not be mixed casually within one product build.

## 7. Release evidence

Every binary release should retain:

- build identifier and source revision;
- target operating system and architecture;
- exact Qt version and module list;
- exact SoapySDR version and redistributed modules;
- dependency inventory or SBOM;
- third-party notice bundle;
- corresponding-source archive identifiers and hashes;
- packaging and license-gate result.

This policy defines engineering release gates. It is not a substitute for legal
review when a distribution model presents unresolved licensing questions.
