# Replacing the Qt shared libraries

SDR Calibration uses Qt through dynamic shared libraries. You may replace the
Qt libraries in this package with interface-compatible modified builds of the
same Qt release.

On macOS, copy the application out of the read-only DMG, replace the Qt
frameworks and Qt plug-ins below `sdrcal-gui.app/Contents`, and re-sign the
modified application with an identity under your control. Apple notarization
and the project's Developer ID signature apply only to the unmodified package;
they are not a technical restriction on running a locally re-signed build.

On Windows, use MSI administrative extraction or install the application, then
replace the Qt DLLs beside the executables and the Qt plug-in DLLs in their
named subdirectories. Authenticode signatures apply only to the original
files. Windows may warn about modified or unsigned files, but the MSI does not
prevent their replacement or execution under the user's normal system policy.

On Ubuntu, the DEB does not contain Qt libraries. They are dynamically loaded
from the declared Ubuntu system packages and can be replaced through the
system package manager or with interface-compatible libraries selected by the
dynamic loader under the user's normal system policy.

The corresponding Qt source shipped with a macOS or Windows package contains
the build system, license texts, and module sources. SDR Calibration does not
modify Qt. These instructions do not promise ABI compatibility with an
arbitrary Qt version or preserve the original package's signatures.
