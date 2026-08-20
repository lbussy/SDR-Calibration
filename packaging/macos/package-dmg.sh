#!/bin/bash

set -euo pipefail

usage() {
    echo "usage: $0 --build-dir DIR --output-dir DIR --signing-identity ID --notary-profile PROFILE --version VERSION --source-dir DIR --qt-source-archive FILE --qt-source-sha256 SHA256 [--qt-additional-source-archives LIST --qt-additional-source-sha256 LIST]" >&2
}

build_dir=
output_dir=
signing_identity=
notary_profile=
version=
source_dir=
qt_source_archive=
qt_source_sha256=
qt_additional_source_archives=
qt_additional_source_sha256=
while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-dir) build_dir=${2-}; shift 2 ;;
        --output-dir) output_dir=${2-}; shift 2 ;;
        --signing-identity) signing_identity=${2-}; shift 2 ;;
        --notary-profile) notary_profile=${2-}; shift 2 ;;
        --version) version=${2-}; shift 2 ;;
        --source-dir) source_dir=${2-}; shift 2 ;;
        --qt-source-archive) qt_source_archive=${2-}; shift 2 ;;
        --qt-source-sha256) qt_source_sha256=${2-}; shift 2 ;;
        --qt-additional-source-archives) qt_additional_source_archives=${2-}; shift 2 ;;
        --qt-additional-source-sha256) qt_additional_source_sha256=${2-}; shift 2 ;;
        *) usage; exit 2 ;;
    esac
done

for value in "$build_dir" "$output_dir" "$signing_identity" "$notary_profile" "$version" "$source_dir" "$qt_source_archive" "$qt_source_sha256"; do
    if [[ -z "$value" ]]; then
        usage
        exit 2
    fi
done

for tool in awk clang cmake cmp codesign ditto file find git grep hdiutil otool plutil realpath sed shasum spctl sw_vers xcrun; do
    command -v "$tool" >/dev/null || { echo "required tool is unavailable: $tool" >&2; exit 1; }
done

qt_prefix=$(sed -n 's#^Qt6_DIR:[^=]*=\(.*\)/lib/cmake/Qt6$#\1#p' \
    "$build_dir/CMakeCache.txt")
macdeployqt="$qt_prefix/bin/macdeployqt"
if [[ -z "$qt_prefix" || ! -x "$macdeployqt" ]]; then
    echo "macdeployqt for the configured Qt was not found" >&2
    exit 1
fi
qt_physical_prefix=$(realpath "$qt_prefix")
while IFS='=' read -r qt_cache_key qt_cache_dir; do
    [[ -d "$qt_cache_dir" ]] || {
        echo "configured Qt package directory does not exist: $qt_cache_key=$qt_cache_dir" >&2
        exit 1
    }
    qt_cache_physical=$(realpath "$qt_cache_dir")
    case "$qt_cache_physical/" in
        "$qt_physical_prefix"/*/) ;;
        *)
            echo "configured Qt package is outside the physical Qt prefix: $qt_cache_key=$qt_cache_dir" >&2
            exit 1
            ;;
    esac
done < <(sed -n 's#^\(Qt6[^:]*_DIR\):[^=]*=\(.*\)$#\1=\2#p' "$build_dir/CMakeCache.txt")

# A split package-manager Qt can satisfy CMake with modules from prefixes that
# macdeployqt does not search.  Reject that configuration before staging,
# signing, or notarization rather than producing a partial runtime closure.
qt_svg_framework="$qt_prefix/lib/QtSvg.framework/Versions/A/QtSvg"
if [[ ! -f "$qt_svg_framework" ]]; then
    echo "Qt prefix used by macdeployqt does not contain the required QtSvg framework: $qt_prefix" >&2
    exit 1
fi
macdeployqt_bin_dir=$(dirname "$(realpath "$macdeployqt")")
macdeployqt_root=${macdeployqt_bin_dir%/bin}
qt_svg_dir=$(dirname "$(realpath "$qt_svg_framework")")
case "$qt_svg_dir/" in
    "$macdeployqt_root"/*/) ;;
    *)
        echo "required QtSvg framework is outside the physical Qt prefix used by macdeployqt" >&2
        exit 1
        ;;
esac

source_revision=$(git -C "$source_dir" rev-parse HEAD)
if [[ -n "$(git -C "$source_dir" status --porcelain)" ]]; then
    echo "refusing to package a dirty source tree" >&2
    exit 1
fi

build_dir=${build_dir%/}
output_dir=${output_dir%/}
case "$output_dir/" in
    "$build_dir"/*/) ;;
    *) echo "output directory must be a child of the build directory" >&2; exit 1 ;;
esac
if [[ "$output_dir" == "$build_dir" ]]; then
    echo "output directory must not equal the build directory" >&2
    exit 1
fi
for source_input in "$qt_source_archive" ${qt_additional_source_archives//;/ }; do
    case "$source_input" in
        "$output_dir"/*)
            echo "corresponding-source input must be outside the package output directory: $source_input" >&2
            exit 1
            ;;
    esac
done

stage_dir="$output_dir/stage"
evidence_dir="$output_dir/evidence"
dmg="$output_dir/SDRCalibration-$version-macOS.dmg"
rm -rf "$output_dir"
mkdir -p "$stage_dir" "$evidence_dir"

cmake --install "$build_dir" --prefix "$stage_dir"
if ! cmp -s "$source_dir/assets/icons/icon-manifest.json" \
        "$stage_dir/share/sdrcal/icons/icon-manifest.json"; then
    echo "staged icon provenance manifest differs from the source" >&2
    exit 1
fi
app="$stage_dir/SDR Calibration.app"
if [[ ! -d "$app" ]]; then
    echo "installed application bundle is missing: $app" >&2
    exit 1
fi

ln -s Frameworks "$app/Contents/lib"
"$macdeployqt" "$app" -always-overwrite -no-codesign
gui_executable="$app/Contents/MacOS/sdrcal-gui"
if ! otool -l "$gui_executable" |
        awk '$1 == "cmd" && $2 == "LC_RPATH" { in_rpath = 1; next }
             in_rpath && $1 == "path" && $2 == "@executable_path/../Frameworks" { found = 1 }
             $1 == "cmd" && $2 != "LC_RPATH" { in_rpath = 0 }
             END { exit(found ? 0 : 1) }'; then
    install_name_tool -add_rpath @executable_path/../Frameworks "$gui_executable"
fi
if ! otool -l "$gui_executable" |
        awk '$1 == "cmd" && $2 == "LC_RPATH" { in_rpath = 1; next }
             in_rpath && $1 == "path" && $2 == "@executable_path/../Frameworks" { found = 1 }
             $1 == "cmd" && $2 != "LC_RPATH" { in_rpath = 0 }
             END { exit(found ? 0 : 1) }'; then
    echo "GUI executable cannot resolve bundled Qt frameworks" >&2
    exit 1
fi
ln -s /Applications "$stage_dir/Applications"

while IFS= read -r -d '' nested_code; do
    codesign --force --sign "$signing_identity" --options runtime --timestamp "$nested_code"
done < <(find "$app/Contents/PlugIns" -type f -print0)
while IFS= read -r -d '' framework; do
    codesign --force --sign "$signing_identity" --options runtime --timestamp "$framework"
done < <(find "$app/Contents/Frameworks" -type d -name '*.framework' -print0)
codesign --force --sign "$signing_identity" --options runtime --timestamp \
    "$gui_executable"
codesign --force --sign "$signing_identity" --options runtime --timestamp "$app"
codesign --force --sign "$signing_identity" --options runtime --timestamp \
    "$stage_dir/bin/sdrcal"

runtime_inventory="$evidence_dir/runtime-closure.txt"
: >"$runtime_inventory"
while IFS= read -r -d '' candidate; do
    if file -b "$candidate" | grep -q 'Mach-O'; then
        {
            echo "FILE ${candidate#"$stage_dir"/}"
            shasum -a 256 "$candidate"
            otool -L "$candidate"
        } >>"$runtime_inventory"
    fi
done < <(find "$stage_dir" -type f -print0)

while IFS= read -r payload_code; do
    case "$payload_code" in
        bin/sdrcal|"SDR Calibration.app/Contents/MacOS/sdrcal-gui"|\
        "SDR Calibration.app"/Contents/Frameworks/Qt*.framework/Versions/*/Qt*|\
        "SDR Calibration.app"/Contents/PlugIns/*/libq*.dylib) ;;
        *) echo "deployed Mach-O lacks an exact Qt disposition: $payload_code" >&2; exit 1 ;;
    esac
done < <(sed -n 's/^FILE //p' "$runtime_inventory")

qt_version=$("$qt_prefix/bin/qmake" -query QT_VERSION)
cmake -DSDRCAL_STAGE_DIR="$stage_dir" -DSDRCAL_OUTPUT_DIR="$evidence_dir" \
    -DSDRCAL_PLATFORM=macOS -DSDRCAL_QT_VERSION="$qt_version" \
    -DSDRCAL_QT_SOURCE_ARCHIVE="$qt_source_archive" \
    -DSDRCAL_QT_SOURCE_SHA256="$qt_source_sha256" \
    -DSDRCAL_QT_ADDITIONAL_SOURCE_ARCHIVES="$qt_additional_source_archives" \
    -DSDRCAL_QT_ADDITIONAL_SOURCE_SHA256="$qt_additional_source_sha256" \
    -DSDRCAL_RUNTIME_INVENTORY="$runtime_inventory" \
    -DSDRCAL_REPLACEMENT_INSTRUCTIONS="$source_dir/packaging/licenses/qt-library-replacement.md" \
    -P "$source_dir/packaging/licenses/assemble-qt-disposition.cmake"

while IFS= read -r dependency; do
    case "$dependency" in
        /System/*|/usr/lib/*|@executable_path/*|@loader_path/*|@rpath/*) ;;
        *) echo "non-portable runtime dependency remains: $dependency" >&2; exit 1 ;;
    esac
done < <(sed -n 's/^[[:space:]][[:space:]]*\([^[:space:]]*\).*/\1/p' "$runtime_inventory")

codesign --verify --deep --strict --verbose=2 "$app" 2>"$evidence_dir/app-codesign-verify.txt"
codesign --display --verbose=4 "$app" 2>"$evidence_dir/app-codesign-details.txt"
plutil -convert xml1 -o "$evidence_dir/Info.plist" "$app/Contents/Info.plist"

app_zip="$output_dir/SDRCalibration-notarization.zip"
ditto -c -k --keepParent "$app" "$app_zip"
xcrun notarytool submit "$app_zip" --keychain-profile "$notary_profile" --wait \
    --output-format json >"$evidence_dir/app-notarization.json"
app_notary_status=$(plutil -extract status raw -o - "$evidence_dir/app-notarization.json")
if [[ "$app_notary_status" != "Accepted" ]]; then
    echo "application notarization was not accepted: $app_notary_status" >&2
    exit 1
fi
xcrun stapler staple -v "$app" >"$evidence_dir/app-stapler.txt" 2>&1
xcrun stapler validate -v "$app" >"$evidence_dir/app-stapler-validate.txt" 2>&1
rm "$app_zip"

hdiutil create -quiet -fs HFS+ -volname "SDR Calibration" -srcfolder "$stage_dir" "$dmg"
codesign --force --sign "$signing_identity" --timestamp "$dmg"
codesign --verify --strict --verbose=2 "$dmg" 2>"$evidence_dir/dmg-codesign-verify.txt"

xcrun notarytool submit "$dmg" --keychain-profile "$notary_profile" --wait \
    --output-format json >"$evidence_dir/notarization.json"
notary_status=$(plutil -extract status raw -o - "$evidence_dir/notarization.json")
if [[ "$notary_status" != "Accepted" ]]; then
    echo "notarization was not accepted: $notary_status" >&2
    exit 1
fi

xcrun stapler staple -v "$dmg" >"$evidence_dir/stapler.txt" 2>&1
xcrun stapler validate -v "$dmg" >"$evidence_dir/stapler-validate.txt" 2>&1
spctl --assess --type open --context context:primary-signature --verbose=4 "$dmg" \
    >"$evidence_dir/gatekeeper-dmg.txt" 2>&1

mount_point=$(mktemp -d /tmp/sdrcal-dmg.XXXXXX)
cleanup() {
    hdiutil detach "$mount_point" -quiet >/dev/null 2>&1 || true
    rmdir "$mount_point" >/dev/null 2>&1 || true
}
trap cleanup EXIT
hdiutil attach -quiet -nobrowse -readonly -mountpoint "$mount_point" "$dmg"
spctl --assess --type execute --verbose=4 "$mount_point/SDR Calibration.app" \
    >"$evidence_dir/gatekeeper-app.txt" 2>&1
codesign --verify --deep --strict --verbose=2 "$mount_point/SDR Calibration.app" \
    2>"$evidence_dir/mounted-app-codesign-verify.txt"
"$mount_point/SDR Calibration.app/Contents/MacOS/sdrcal-gui" \
    >"$evidence_dir/mounted-app-launch.stdout" \
    2>"$evidence_dir/mounted-app-launch.stderr" &
launch_pid=$!
sleep 3
if ! kill -0 "$launch_pid" 2>/dev/null; then
    wait "$launch_pid" || true
    echo "mounted application exited during launch smoke test" >&2
    cat "$evidence_dir/mounted-app-launch.stderr" >&2
    exit 1
fi
kill -TERM "$launch_pid"
wait "$launch_pid" || true
cleanup
trap - EXIT

dmg_sha256=$(shasum -a 256 "$dmg" | awk '{print $1}')
cat >"$evidence_dir/manifest.json" <<EOF
{
  "schema_version": 1,
  "source_revision": "$source_revision",
  "project_version": "$version",
  "platform": "macOS $(sw_vers -productVersion)",
  "platform_build": "$(sw_vers -buildVersion)",
  "architecture": "$(uname -m)",
  "artifact": "$(basename "$dmg")",
  "sha256": "$dmg_sha256",
  "qt_version": "$qt_version",
  "cmake_version": "$(cmake --version | awk 'NR == 1 {print $3}')",
  "macos_sdk": "$(xcrun --show-sdk-version)",
  "compiler": "$(clang --version | awk 'NR == 1')",
  "deployment": "macdeployqt dynamic frameworks",
  "signing": "Developer ID Application hardened runtime with secure timestamp",
  "notarization": "Application and DMG accepted and stapled",
  "gatekeeper": "DMG open and mounted application execution assessments passed",
  "launch_smoke": "Mounted application remained running for three seconds without hardware access",
  "hardware_access": false,
  "distribution_license_gate": "passed; see license-manifest.json"
}
EOF

echo "Created signed and notarized DMG: $dmg"
echo "SHA-256: $dmg_sha256"
echo "Evidence: $evidence_dir"
