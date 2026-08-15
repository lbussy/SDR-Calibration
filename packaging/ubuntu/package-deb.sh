#!/bin/bash

set -euo pipefail

usage() {
    echo "usage: $0 --build-dir DIR --output-dir DIR --version VERSION --source-dir DIR" >&2
}

build_dir=
output_dir=
version=
source_dir=
while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-dir) build_dir=${2-}; shift 2 ;;
        --output-dir) output_dir=${2-}; shift 2 ;;
        --version) version=${2-}; shift 2 ;;
        --source-dir) source_dir=${2-}; shift 2 ;;
        *) usage; exit 2 ;;
    esac
done

for value in "$build_dir" "$output_dir" "$version" "$source_dir"; do
    if [[ -z "$value" ]]; then
        usage
        exit 2
    fi
done

for tool in awk basename cmake cp dirname dpkg-deb dpkg-query dpkg-shlibdeps du file find git grep ldd \
    mkdir python3 qtpaths6 readelf rm sed sha256sum sort uname xargs; do
    command -v "$tool" >/dev/null || { echo "required tool is unavailable: $tool" >&2; exit 1; }
done

if [[ ! -r /etc/os-release ]]; then
    echo "Ubuntu release identity is unavailable" >&2
    exit 1
fi
# shellcheck disable=SC1091
source /etc/os-release
if [[ "${ID-}" != "ubuntu" || "${VERSION_ID-}" != "24.04" ]]; then
    echo "Ubuntu 24.04 is required; found ${PRETTY_NAME-unknown}" >&2
    exit 1
fi
if [[ "$(uname -m)" != "x86_64" ]]; then
    echo "x86_64 is required; found $(uname -m)" >&2
    exit 1
fi

source_revision=$(git -C "$source_dir" rev-parse HEAD)
if [[ -n "$(git -C "$source_dir" status --porcelain)" ]]; then
    echo "refusing to package a dirty source tree" >&2
    exit 1
fi

build_dir=$(python3 -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$build_dir")
output_dir=$(python3 -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$output_dir")
source_dir=$(python3 -c 'import os,sys; print(os.path.realpath(sys.argv[1]))' "$source_dir")
if [[ "$(dirname "$output_dir")" != "$build_dir" ]]; then
    echo "output directory must be a direct child of the build directory" >&2
    exit 1
fi

cache=$(cmake -LA -N "$build_dir")
if ! grep -q '^SDRCAL_ENABLE_SOAPYSDR:BOOL=OFF$' <<<"$cache"; then
    echo "Ubuntu production package requires SDRCAL_ENABLE_SOAPYSDR=OFF" >&2
    exit 1
fi
if ! grep -q '^SDRCAL_BUILD_GUI:BOOL=ON$' <<<"$cache"; then
    echo "Ubuntu production package requires SDRCAL_BUILD_GUI=ON" >&2
    exit 1
fi

rm -rf "$output_dir"
stage_dir="$output_dir/stage"
extract_dir="$output_dir/extracted"
control_dir="$output_dir/control"
evidence_dir="$output_dir/evidence"
mkdir -p "$stage_dir" "$extract_dir" "$control_dir" "$evidence_dir"

DESTDIR="$stage_dir" cmake --install "$build_dir" --prefix /usr --strip

for required in usr/bin/sdrcal usr/bin/sdrcal-gui usr/share/sdrcal/LICENSE \
    usr/share/sdrcal/THIRD_PARTY_NOTICES.md usr/share/sdrcal/sdrcal.spdx.json \
    usr/share/applications/sdrcal.desktop; do
    if [[ ! -f "$stage_dir/$required" ]]; then
        echo "required Debian payload is missing: $required" >&2
        exit 1
    fi
done
if [[ -e "$stage_dir/usr/bin/sdrcal-capture" ]]; then
    echo "SoapySDR capture executable must not enter the Ubuntu production package" >&2
    exit 1
fi
if find "$stage_dir" -type f \( -name '*.so' -o -name '*.so.*' \) -print -quit | grep -q .; then
    echo "Ubuntu production package must not convey a shared library" >&2
    exit 1
fi

mkdir -p "$stage_dir/DEBIAN" "$output_dir/debian"
cat >"$output_dir/debian/control" <<EOF
Source: sdrcal
Section: science
Priority: optional
Maintainer: SDR Calibration contributors <lee@bussy.org>

Package: sdrcal
Architecture: amd64
Description: Traceable per-device SDR frequency calibration
EOF
(
    cd "$output_dir"
    dpkg-shlibdeps -psdrcal -O -e"$stage_dir/usr/bin/sdrcal" \
        -e"$stage_dir/usr/bin/sdrcal-gui" 2>"$evidence_dir/dpkg-shlibdeps.txt"
) | sed -n 's/^[^:]*:Depends=//p' >"$evidence_dir/dependencies.txt"
depends=$(<"$evidence_dir/dependencies.txt")
if [[ -z "$depends" || "$depends" != *libqt6widgets6* ]]; then
    echo "derived Debian dependencies are missing the Qt Widgets runtime" >&2
    exit 1
fi

license_dir="$stage_dir/usr/share/sdrcal/license-disposition"
mkdir -p "$license_dir"
python3 - "$depends" "$evidence_dir/dependency-license-disposition.tsv" <<'PY'
import pathlib
import re
import subprocess
import sys

rows = []
for clause in sys.argv[1].split(","):
    alternatives = []
    for alternative in clause.split("|"):
        match = re.match(r"\s*([a-z0-9][a-z0-9+.-]*(?::[a-z0-9]+)?)", alternative)
        if match:
            alternatives.append(match.group(1))
    selected = None
    for package in alternatives:
        query = subprocess.run(
            ["dpkg-query", "-W", "-f=${Package}\t${Version}\n", package],
            text=True, capture_output=True)
        if query.returncode == 0:
            selected = query.stdout.strip().split("\t", 1)
            break
    if selected is None:
        raise SystemExit(f"no installed package satisfies dependency clause: {clause.strip()}")
    package, version = selected
    copyright_file = pathlib.Path("/usr/share/doc") / package.split(":", 1)[0] / "copyright"
    if not copyright_file.is_file():
        raise SystemExit(f"installed dependency lacks a copyright disposition: {package}")
    rows.append((clause.strip(), package, version, str(copyright_file)))

with open(sys.argv[2], "w", encoding="utf-8", newline="\n") as output:
    output.write("dependency_clause\tinstalled_package\tversion\tcopyright_file\n")
    for row in rows:
        output.write("\t".join(row) + "\n")
PY
cp "$evidence_dir/dependency-license-disposition.tsv" "$license_dir/"
cp "$source_dir/packaging/licenses/qt-library-replacement.md" \
    "$license_dir/QT_LIBRARY_REPLACEMENT.md"
cat >"$license_dir/README.md" <<EOF
# Exact binary-license disposition

This Ubuntu DEB conveys only MIT-licensed SDR Calibration project files. It
does not contain Qt, SoapySDR, an SDR vendor module, or another third-party
shared library. Qt and the remaining runtime libraries are dynamically loaded
from the exact Ubuntu packages recorded in
\`dependency-license-disposition.tsv\`; the package versions and their
installed Debian copyright records were verified while constructing this DEB.
Those system packages are not redistributed inside this archive. See
\`QT_LIBRARY_REPLACEMENT.md\` for the replacement path.
EOF

installed_size=$(du -sk "$stage_dir/usr" | awk '{print $1}')
cat >"$stage_dir/DEBIAN/control" <<EOF
Package: sdrcal
Version: $version
Architecture: amd64
Maintainer: SDR Calibration contributors <lee@bussy.org>
Section: science
Priority: optional
Installed-Size: $installed_size
Depends: $depends
Description: Traceable per-device SDR frequency calibration
 Hardware-free recorded-input CLI and Qt desktop application.
EOF

deb="$output_dir/sdrcal_${version}_amd64.deb"
dpkg-deb --root-owner-group --build "$stage_dir" "$deb" >"$evidence_dir/dpkg-deb-build.txt"
dpkg-deb --info "$deb" >"$evidence_dir/dpkg-deb-info.txt"
dpkg-deb --contents "$deb" >"$evidence_dir/dpkg-deb-contents.txt"
dpkg-deb --field "$deb" >"$evidence_dir/control.txt"
dpkg-deb --extract "$deb" "$extract_dir"
dpkg-deb --control "$deb" "$control_dir"

for required in usr/bin/sdrcal usr/bin/sdrcal-gui usr/share/sdrcal/LICENSE \
    usr/share/sdrcal/THIRD_PARTY_NOTICES.md usr/share/sdrcal/sdrcal.spdx.json \
    usr/share/sdrcal/license-disposition/README.md \
    usr/share/sdrcal/license-disposition/dependency-license-disposition.tsv \
    usr/share/sdrcal/license-disposition/QT_LIBRARY_REPLACEMENT.md \
    usr/share/applications/sdrcal.desktop; do
    if [[ ! -f "$extract_dir/$required" ]]; then
        echo "required extracted Debian payload is missing: $required" >&2
        exit 1
    fi
done
if grep -R -F "$build_dir" "$extract_dir/usr/share/sdrcal" "$control_dir"; then
    echo "Debian package metadata leaks the build-tree path" >&2
    exit 1
fi

"$extract_dir/usr/bin/sdrcal" --help >/dev/null
for binary in "$extract_dir/usr/bin/sdrcal" "$extract_dir/usr/bin/sdrcal-gui"; do
    if ! file "$binary" | grep -q 'ELF 64-bit.*x86-64'; then
        echo "unexpected Debian payload architecture: $binary" >&2
        exit 1
    fi
    readelf -d "$binary" >"$evidence_dir/$(basename "$binary")-dynamic.txt"
    if grep -E 'RPATH|RUNPATH' "$evidence_dir/$(basename "$binary")-dynamic.txt"; then
        echo "Debian payload contains an RPATH or RUNPATH: $binary" >&2
        exit 1
    fi
    ldd "$binary" >"$evidence_dir/$(basename "$binary")-ldd.txt"
    if grep -q 'not found' "$evidence_dir/$(basename "$binary")-ldd.txt"; then
        echo "Debian payload has an unresolved runtime dependency: $binary" >&2
        exit 1
    fi
done

find "$extract_dir" -type f -print0 | sort -z | xargs -0 sha256sum \
    | sed "s#$extract_dir/##" >"$evidence_dir/payload-sha256.txt"
deb_sha256=$(sha256sum "$deb" | awk '{print $1}')
qt_version=$(qtpaths6 --qt-version)
compiler=$(cmake -LA -N "$build_dir" | sed -n 's/^CMAKE_CXX_COMPILER:FILEPATH=//p')
python3 - "$evidence_dir/manifest.json" <<PY
import json
import subprocess
import sys

manifest = {
    "schema_version": 1,
    "source_revision": "$source_revision",
    "project_version": "$version",
    "platform": "${PRETTY_NAME}",
    "architecture": "amd64",
    "artifact": "$(basename "$deb")",
    "sha256": "$deb_sha256",
    "qt_version": "$qt_version",
    "cmake_version": subprocess.check_output(["cmake", "--version"], text=True).splitlines()[0],
    "compiler": subprocess.check_output(["$compiler", "--version"], text=True).splitlines()[0],
    "dependency_policy": "dpkg-shlibdeps derived system runtime dependencies",
    "clean_install_qualified": False,
    "distribution_license_gate": "passed for the exact DEB payload",
    "device_qualified": False,
    "hardware_access": False,
}
with open(sys.argv[1], "w", encoding="utf-8") as output:
    json.dump(manifest, output, indent=2, sort_keys=True)
    output.write("\n")
PY

echo "Created audited Ubuntu DEB: $deb"
echo "SHA-256: $deb_sha256"
echo "Evidence: $evidence_dir"
