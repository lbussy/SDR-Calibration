if(NOT DEFINED SDRCAL_BUILD_DIR OR NOT DEFINED SDRCAL_SOURCE_DIR
        OR NOT DEFINED SDRCAL_STAGE_DIR)
    message(FATAL_ERROR
        "SDRCAL_BUILD_DIR, SDRCAL_SOURCE_DIR, and SDRCAL_STAGE_DIR are required")
endif()

file(REMOVE_RECURSE "${SDRCAL_STAGE_DIR}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${SDRCAL_BUILD_DIR}" --prefix "${SDRCAL_STAGE_DIR}"
    RESULT_VARIABLE install_result
)
if(NOT install_result EQUAL 0)
    message(FATAL_ERROR "isolated package install failed: ${install_result}")
endif()

set(required_files
    "${SDRCAL_STAGE_DIR}/${SDRCAL_BINDIR}/sdrcal${SDRCAL_EXECUTABLE_SUFFIX}"
    "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/sdrcal/LICENSE"
    "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/sdrcal/THIRD_PARTY_NOTICES.md"
    "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/sdrcal/sdrcal.spdx.json"
    "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/sdrcal/icons/README.md"
    "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/sdrcal/icons/icon-manifest.json"
)
if(SDRCAL_EXPECT_CAPTURE)
    list(APPEND required_files
        "${SDRCAL_STAGE_DIR}/${SDRCAL_BINDIR}/sdrcal-capture${SDRCAL_EXECUTABLE_SUFFIX}")
endif()
if(SDRCAL_EXPECT_GUI)
    if(SDRCAL_GUI_IS_BUNDLE)
        list(APPEND required_files
            "${SDRCAL_STAGE_DIR}/SDR Calibration.app"
            "${SDRCAL_STAGE_DIR}/SDR Calibration.app/Contents/MacOS/sdrcal-gui"
            "${SDRCAL_STAGE_DIR}/SDR Calibration.app/Contents/Resources/SDRCalibration.icns")
        if(EXISTS "${SDRCAL_STAGE_DIR}/sdrcal-gui.app")
            message(FATAL_ERROR "stale user-facing macOS bundle name was installed")
        endif()
    else()
        list(APPEND required_files
            "${SDRCAL_STAGE_DIR}/${SDRCAL_BINDIR}/sdrcal-gui${SDRCAL_EXECUTABLE_SUFFIX}")
        if(UNIX AND NOT APPLE)
            list(APPEND required_files
                "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/applications/sdrcal.desktop")
            foreach(icon_size 16 24 32 48 64 128 256 512 1024)
                list(APPEND required_files
                    "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/icons/hicolor/${icon_size}x${icon_size}/apps/sdr-calibration.png")
            endforeach()
        endif()
    endif()
endif()
foreach(required_file IN LISTS required_files)
    if(NOT EXISTS "${required_file}")
        message(FATAL_ERROR "required staged package content is missing: ${required_file}")
    endif()
endforeach()

file(SHA256 "${SDRCAL_SOURCE_DIR}/assets/icons/icon-manifest.json" source_icon_manifest_hash)
file(SHA256
    "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/sdrcal/icons/icon-manifest.json"
    staged_icon_manifest_hash)
if(NOT source_icon_manifest_hash STREQUAL staged_icon_manifest_hash)
    message(FATAL_ERROR "installed icon provenance manifest differs from the source")
endif()

if(SDRCAL_EXPECT_GUI AND SDRCAL_GUI_IS_BUNDLE)
    file(READ "${SDRCAL_STAGE_DIR}/SDR Calibration.app/Contents/Info.plist" bundle_plist)
    foreach(required_plist_fragment
            "<key>CFBundleExecutable</key>"
            "<string>sdrcal-gui</string>"
            "<key>CFBundleName</key>"
            "<string>SDR Calibration</string>"
            "<key>CFBundleIconFile</key>"
            "<string>SDRCalibration.icns</string>")
        string(FIND "${bundle_plist}" "${required_plist_fragment}" plist_fragment_offset)
        if(plist_fragment_offset EQUAL -1)
            message(FATAL_ERROR
                "installed macOS bundle metadata is missing: ${required_plist_fragment}")
        endif()
    endforeach()
endif()

file(READ "${SDRCAL_STAGE_DIR}/${SDRCAL_DATADIR}/sdrcal/sdrcal.spdx.json" spdx)
string(JSON spdx_version ERROR_VARIABLE json_error GET "${spdx}" spdxVersion)
if(json_error OR NOT spdx_version STREQUAL "SPDX-2.3")
    message(FATAL_ERROR "installed SPDX inventory is invalid: ${json_error}")
endif()
string(FIND "${spdx}" "${SDRCAL_BUILD_DIR}" build_path_offset)
if(NOT build_path_offset EQUAL -1)
    message(FATAL_ERROR "installed metadata leaks the build-tree path")
endif()

execute_process(
    COMMAND "${SDRCAL_STAGE_DIR}/${SDRCAL_BINDIR}/sdrcal${SDRCAL_EXECUTABLE_SUFFIX}" --help
    RESULT_VARIABLE cli_result
    OUTPUT_QUIET
    ERROR_QUIET
)
if(NOT cli_result EQUAL 0)
    message(FATAL_ERROR "staged sdrcal CLI startup check failed: ${cli_result}")
endif()

message(STATUS
    "Package content and CLI startup passed. Runtime dependency deployment, clean-host "
    "installation, native installer, and signing checks remain separate qualification gates.")
