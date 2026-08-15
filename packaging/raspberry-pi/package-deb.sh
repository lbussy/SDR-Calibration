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

for tool in awk basename cmake df dpkg-deb dpkg-query dpkg-shlibdeps du file find git grep ldd \
    python3 readelf sed sha256sum sort uname xargs; do
    command -v "$tool" >/dev/null || {
        echo "required tool is unavailable: $tool" >&2
        exit 1
    }
done

if [[ ! -r /etc/os-release || ! -r /etc/rpi-issue ]]; then
    echo "64-bit Raspberry Pi OS identity is unavailable" >&2
    exit 1
fi
# shellcheck disable=SC1091
source /etc/os-release
if [[ "${ID-}" != "debian" || "${VERSION_ID-}" != "13" || \
      "$(uname -m)" != "aarch64" ]]; then
    echo "64-bit Raspberry Pi OS 13 is required; found ${PRETTY_NAME-unknown} $(uname -m)" >&2
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
if [[ "$(python3 -c 'import os,sys; print(os.path.dirname(sys.argv[1]))' "$output_dir")" != \
      "$build_dir" ]]; then
    echo "output directory must be a direct child of the build directory" >&2
    exit 1
fi

cache=$(cmake -LA -N "$build_dir")
for required in \
    'SDRCAL_BUILD_CLI:BOOL=ON' \
    'SDRCAL_BUILD_GUI:BOOL=OFF' \
    'SDRCAL_ENABLE_SOAPYSDR:BOOL=OFF' \
    'SDRCAL_ENABLE_PACKAGING:BOOL=ON' \
    'SDRCAL_RECORDED_INPUT_MAX_BYTES:STRING=134217728'; do
    if ! grep -q "^${required}$" <<<"$cache"; then
        echo "Raspberry Pi production package requires ${required}" >&2
        exit 1
    fi
done

rm -rf "$output_dir"
stage_dir="$output_dir/stage"
extract_dir="$output_dir/extracted"
control_dir="$output_dir/control"
evidence_dir="$output_dir/evidence"
mkdir -p "$stage_dir" "$extract_dir" "$control_dir" "$evidence_dir"

DESTDIR="$stage_dir" cmake --install "$build_dir" --prefix /usr --strip

for required in usr/bin/sdrcal usr/share/sdrcal/LICENSE \
    usr/share/sdrcal/THIRD_PARTY_NOTICES.md usr/share/sdrcal/sdrcal.spdx.json \
    usr/share/sdrcal/schemas/sdr-calibration-profile.schema.json; do
    if [[ ! -f "$stage_dir/$required" ]]; then
        echo "required Raspberry Pi payload is missing: $required" >&2
        exit 1
    fi
done
for forbidden in usr/bin/sdrcal-gui usr/bin/sdrcal-capture \
    usr/share/applications/sdrcal.desktop usr/share/icons; do
    if [[ -e "$stage_dir/$forbidden" ]]; then
        echo "forbidden CLI-only payload is present: $forbidden" >&2
        exit 1
    fi
done
if find "$stage_dir" -type f \( -name '*.so' -o -name '*.so.*' \) -print -quit | grep -q .; then
    echo "Raspberry Pi CLI package must not convey a shared library" >&2
    exit 1
fi
mkdir -p "$stage_dir/DEBIAN" "$output_dir/debian"
cat >"$output_dir/debian/control" <<EOF
Source: sdrcal
Section: science
Priority: optional
Maintainer: SDR Calibration contributors <lee@bussy.org>

Package: sdrcal
Architecture: arm64
Description: Traceable per-device SDR frequency calibration CLI
EOF
(
    cd "$output_dir"
    dpkg-shlibdeps -psdrcal -O -e"$stage_dir/usr/bin/sdrcal" \
        2>"$evidence_dir/dpkg-shlibdeps.txt"
) | sed -n 's/^[^:]*:Depends=//p' >"$evidence_dir/dependencies.txt"
depends=$(<"$evidence_dir/dependencies.txt")
if [[ -z "$depends" || "$depends" == *qt* || "$depends" == *soapysdr* ]]; then
    echo "derived Raspberry Pi dependencies are empty or contain a forbidden runtime" >&2
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
            text=True, capture_output=True, check=False)
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
cat >"$license_dir/README.md" <<EOF
# Exact binary-license disposition

This Raspberry Pi OS ARM64 DEB conveys only MIT-licensed SDR Calibration project
files. It does not contain Qt, SoapySDR, an SDR vendor module, vendor runtime, or
another third-party shared library. Runtime libraries are dynamically loaded
from the exact system packages recorded in
\`dependency-license-disposition.tsv\`. Those packages are not redistributed
inside this archive.
EOF

installed_size=$(du -sk "$stage_dir/usr" | awk '{print $1}')
cat >"$stage_dir/DEBIAN/control" <<EOF
Package: sdrcal
Version: $version
Architecture: arm64
Maintainer: SDR Calibration contributors <lee@bussy.org>
Section: science
Priority: optional
Installed-Size: $installed_size
Depends: $depends
Description: Traceable per-device SDR frequency calibration CLI
 Hardware-free recorded-input command-line workflow for 64-bit Raspberry Pi OS.
EOF

deb="$output_dir/sdrcal_${version}_arm64.deb"
dpkg-deb --root-owner-group --build "$stage_dir" "$deb" >"$evidence_dir/dpkg-deb-build.txt"
dpkg-deb --info "$deb" >"$evidence_dir/dpkg-deb-info.txt"
dpkg-deb --contents "$deb" >"$evidence_dir/dpkg-deb-contents.txt"
dpkg-deb --field "$deb" >"$evidence_dir/control.txt"
dpkg-deb --extract "$deb" "$extract_dir"
dpkg-deb --control "$deb" "$control_dir"

for required in usr/bin/sdrcal usr/share/sdrcal/LICENSE \
    usr/share/sdrcal/THIRD_PARTY_NOTICES.md usr/share/sdrcal/sdrcal.spdx.json \
    usr/share/sdrcal/schemas/sdr-calibration-profile.schema.json \
    usr/share/sdrcal/license-disposition/README.md \
    usr/share/sdrcal/license-disposition/dependency-license-disposition.tsv; do
    if [[ ! -f "$extract_dir/$required" ]]; then
        echo "required extracted Raspberry Pi payload is missing: $required" >&2
        exit 1
    fi
done
if grep -R -F "$build_dir" "$extract_dir/usr/share/sdrcal" "$control_dir"; then
    echo "Raspberry Pi package metadata leaks the build-tree path" >&2
    exit 1
fi

binary="$extract_dir/usr/bin/sdrcal"
"$binary" --help >/dev/null
if ! file "$binary" | grep -q 'ELF 64-bit.*ARM aarch64'; then
    echo "unexpected Raspberry Pi payload architecture: $binary" >&2
    exit 1
fi
readelf -d "$binary" >"$evidence_dir/sdrcal-dynamic.txt"
if grep -E 'RPATH|RUNPATH' "$evidence_dir/sdrcal-dynamic.txt"; then
    echo "Raspberry Pi payload contains an RPATH or RUNPATH" >&2
    exit 1
fi
ldd "$binary" >"$evidence_dir/sdrcal-ldd.txt"
if grep -q 'not found' "$evidence_dir/sdrcal-ldd.txt"; then
    echo "Raspberry Pi payload has an unresolved runtime dependency" >&2
    exit 1
fi
if grep -Ei 'Qt6|SoapySDR|Soapy[A-Za-z0-9_-]*' "$evidence_dir/sdrcal-ldd.txt"; then
    echo "Qt or SoapySDR leaked into the Raspberry Pi CLI runtime closure" >&2
    exit 1
fi

find "$extract_dir" -type f -print0 | sort -z | xargs -0 sha256sum \
    | sed "s#$extract_dir/##" >"$evidence_dir/payload-sha256.txt"
deb_sha256=$(sha256sum "$deb" | awk '{print $1}')
compiler=$(cmake -LA -N "$build_dir" | sed -n 's/^CMAKE_CXX_COMPILER:FILEPATH=//p')
cmake -LA -N "$build_dir" >"$evidence_dir/cmake-cache.txt"
{
    uname -a
    cat /proc/device-tree/model
    printf '\n'
    cat /etc/os-release
    cat /etc/rpi-issue
    grep '^MemTotal:' /proc/meminfo
    df -h / "$build_dir"
    cmake --version | head -1
    "$compiler" --version | head -1
    dpkg-deb --version | head -1
} >"$evidence_dir/host-environment.txt"
python3 - "$evidence_dir/manifest.json" "$source_dir" "$build_dir" <<PY
import json
import pathlib
import platform
import subprocess
import sys

manifest = {
    "schema_version": 1,
    "source_revision": "$source_revision",
    "project_version": "$version",
    "platform": "${PRETTY_NAME}",
    "architecture": "arm64",
    "board_model": pathlib.Path("/proc/device-tree/model").read_bytes().rstrip(b"\0").decode(),
    "memory_total_kib": int(next(line.split()[1] for line in
        pathlib.Path("/proc/meminfo").read_text().splitlines() if line.startswith("MemTotal:"))),
    "kernel": platform.release(),
    "rpi_issue": pathlib.Path("/etc/rpi-issue").read_text(encoding="utf-8").strip(),
    "artifact": "$(basename "$deb")",
    "sha256": "$deb_sha256",
    "recorded_input_maximum_bytes": 134217728,
    "cmake_version": subprocess.check_output(["cmake", "--version"], text=True).splitlines()[0],
    "compiler": subprocess.check_output(["$compiler", "--version"], text=True).splitlines()[0],
    "dependency_policy": "dpkg-shlibdeps derived system runtime dependencies",
    "clean_install_qualified": False,
    "device_qualified": False,
    "hardware_access": False,
}
with open(sys.argv[1], "w", encoding="utf-8") as output:
    json.dump(manifest, output, indent=2, sort_keys=True)
    output.write("\n")
PY

echo "Created audited Raspberry Pi OS DEB: $deb"
echo "SHA-256: $deb_sha256"
echo "Evidence: $evidence_dir"
