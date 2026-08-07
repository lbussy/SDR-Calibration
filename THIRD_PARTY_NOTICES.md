# Third-party notices

The MIT License in `LICENSE` applies only to original SDR Calibration project
code and documentation unless a file states otherwise. It does not relicense
third-party components.

## Qt 6

The application is intended to use Qt 6 modules available under the GNU Lesser
General Public License version 3, dynamically linked by default. Qt is
copyright its respective copyright holders and is not covered by this
project's MIT License.

Initial permitted Qt module set:

- Qt Core
- Qt Widgets
- Qt Network
- Qt Concurrent
- Qt OpenGLWidgets, when required

Before distributing binaries, release packaging must include the license texts,
notices, corresponding-source offer or controlled source archive, replacement
instructions, and other materials required by the exact Qt version and modules
being distributed.

GPL-only Qt modules must not be introduced without an explicit project license
and distribution review. Qt Graphs is not part of the initial permitted module
set.

This file will be expanded with exact versions, copyright notices, source
archive locations, and transitive third-party licenses when dependencies are
introduced and before the first binary distribution.

## SoapySDR and device modules

SoapySDR, its dependencies, and separately installed vendor device modules
retain their own licenses. Their exact notices and redistribution requirements
must be recorded when build and packaging dependencies are selected.
