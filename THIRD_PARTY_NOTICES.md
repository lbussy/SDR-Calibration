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

The macOS and Windows release gates convey the exact deployed Qt closure under
LGPL-3.0-only and require the complete, hash-pinned qtbase source archive to
accompany each binary package. That archive supplies the authoritative license,
copyright, and file-level REUSE metadata for Qt and its embedded third-party
code. The package also records the deployed-code inventory and shared-library
replacement instructions. SDR Calibration makes no modifications to Qt.

The Ubuntu DEB does not convey Qt binaries. Its exact system-package
dependencies, installed versions, and Debian copyright-file locations are
recorded in its license-disposition directory.

GPL-only Qt modules must not be introduced without an explicit project license
and distribution review. Qt Graphs is not part of the initial permitted module
set.

The exact version, source-archive name and hash, runtime inventory, and platform
disposition are generated from each release payload and must not be inferred
from this repository-level notice.

## SoapySDR and device modules

SoapySDR, its dependencies, and separately installed vendor device modules
retain their own licenses. Their exact notices and redistribution requirements
must be recorded when build and packaging dependencies are selected.
