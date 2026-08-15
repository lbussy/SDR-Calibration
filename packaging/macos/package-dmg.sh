#!/bin/bash

set -euo pipefail

usage() {
    echo "usage: $0 --build-dir DIR --output-dir DIR --signing-identity ID --notary-profile PROFILE --version VERSION --source-dir DIR" >&2
}

build_dir=
output_dir=
signing_identity=
notary_profile=
version=
source_dir=
while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-dir) build_dir=${2-}; shift 2 ;;
        --output-dir) output_dir=${2-}; shift 2 ;;
        --signing-identity) signing_identity=${2-}; shift 2 ;;
        --notary-profile) notary_profile=${2-}; shift 2 ;;
        --version) version=${2-}; shift 2 ;;
        --source-dir) source_dir=${2-}; shift 2 ;;
        *) usage; exit 2 ;;
    esac
done

for value in "$build_dir" "$output_dir" "$signing_identity" "$notary_profile" "$version" "$source_dir"; do
    if [[ -z "$value" ]]; then
        usage
        exit 2
    fi
done

for tool in awk clang cmake codesign ditto file find git grep hdiutil otool plutil sed shasum spctl sw_vers xcrun; do
    command -v "$tool" >/dev/null || { echo "required tool is unavailable: $tool" >&2; exit 1; }
done

qt_prefix=$(cmake -LA -N "$build_dir" | sed -n 's#^Qt6_DIR:PATH=\(.*\)/lib/cmake/Qt6$#\1#p')
macdeployqt="$qt_prefix/bin/macdeployqt"
if [[ -z "$qt_prefix" || ! -x "$macdeployqt" ]]; then
    echo "macdeployqt for the configured Qt was not found" >&2
    exit 1
fi

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

stage_dir="$output_dir/stage"
evidence_dir="$output_dir/evidence"
dmg="$output_dir/SDRCalibration-$version-macOS.dmg"
rm -rf "$output_dir"
mkdir -p "$stage_dir" "$evidence_dir"

cmake --install "$build_dir" --prefix "$stage_dir"
app="$stage_dir/sdrcal-gui.app"
if [[ ! -d "$app" ]]; then
    echo "installed application bundle is missing: $app" >&2
    exit 1
fi

"$macdeployqt" "$app" -always-overwrite -sign-for-notarization="$signing_identity"
ln -s /Applications "$stage_dir/Applications"

while IFS= read -r -d '' executable; do
    codesign --force --sign "$signing_identity" --options runtime --timestamp "$executable"
done < <(find "$stage_dir" -type f -perm -111 ! -path "$app/*" -print0)

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

while IFS= read -r dependency; do
    case "$dependency" in
        /System/*|/usr/lib/*|@executable_path/*|@loader_path/*|@rpath/*) ;;
        *) echo "non-portable runtime dependency remains: $dependency" >&2; exit 1 ;;
    esac
done < <(sed -n 's/^[[:space:]]*\([^[:space:]]*\).*/\1/p' "$runtime_inventory")

codesign --verify --deep --strict --verbose=2 "$app" 2>"$evidence_dir/app-codesign-verify.txt"
codesign --display --verbose=4 "$app" 2>"$evidence_dir/app-codesign-details.txt"
plutil -convert xml1 -o "$evidence_dir/Info.plist" "$app/Contents/Info.plist"

app_zip="$output_dir/sdrcal-gui-notarization.zip"
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
spctl --assess --type execute --verbose=4 "$mount_point/sdrcal-gui.app" \
    >"$evidence_dir/gatekeeper-app.txt" 2>&1
codesign --verify --deep --strict --verbose=2 "$mount_point/sdrcal-gui.app" \
    2>"$evidence_dir/mounted-app-codesign-verify.txt"
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
  "qt_version": "$("$qt_prefix/bin/qmake" -query QT_VERSION)",
  "cmake_version": "$(cmake --version | awk 'NR == 1 {print $3}')",
  "macos_sdk": "$(xcrun --show-sdk-version)",
  "compiler": "$(clang --version | awk 'NR == 1')",
  "deployment": "macdeployqt dynamic frameworks",
  "signing": "Developer ID Application hardened runtime with secure timestamp",
  "notarization": "Application and DMG accepted and stapled",
  "gatekeeper": "DMG open and mounted application execution assessments passed",
  "hardware_access": false
}
EOF

echo "Created signed and notarized DMG: $dmg"
echo "SHA-256: $dmg_sha256"
echo "Evidence: $evidence_dir"
