cmake_minimum_required(VERSION 3.20)

foreach(required SDRCAL_STAGE_DIR SDRCAL_OUTPUT_DIR SDRCAL_PLATFORM
        SDRCAL_QT_VERSION SDRCAL_QT_SOURCE_ARCHIVE SDRCAL_QT_SOURCE_SHA256
        SDRCAL_RUNTIME_INVENTORY SDRCAL_REPLACEMENT_INSTRUCTIONS)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "missing required license-disposition input: ${required}")
    endif()
endforeach()

if(NOT SDRCAL_PLATFORM MATCHES "^(macOS|Windows)$")
    message(FATAL_ERROR "bundled Qt disposition is only valid for macOS or Windows")
endif()
foreach(path SDRCAL_QT_SOURCE_ARCHIVE SDRCAL_RUNTIME_INVENTORY
        SDRCAL_REPLACEMENT_INSTRUCTIONS)
    if(NOT EXISTS "${${path}}")
        message(FATAL_ERROR "license-disposition input does not exist: ${${path}}")
    endif()
endforeach()
string(LENGTH "${SDRCAL_QT_SOURCE_SHA256}" source_sha256_length)
if(NOT source_sha256_length EQUAL 64 OR
   NOT SDRCAL_QT_SOURCE_SHA256 MATCHES "^[0-9a-fA-F]+$")
    message(FATAL_ERROR "Qt source SHA-256 must contain exactly 64 hexadecimal characters")
endif()

file(SHA256 "${SDRCAL_QT_SOURCE_ARCHIVE}" actual_source_sha256)
string(TOLOWER "${SDRCAL_QT_SOURCE_SHA256}" expected_source_sha256)
if(NOT actual_source_sha256 STREQUAL expected_source_sha256)
    message(FATAL_ERROR
        "Qt source SHA-256 mismatch: expected ${expected_source_sha256}, got ${actual_source_sha256}")
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E tar tf "${SDRCAL_QT_SOURCE_ARCHIVE}"
    RESULT_VARIABLE archive_result OUTPUT_VARIABLE archive_listing ERROR_VARIABLE archive_error)
if(NOT archive_result EQUAL 0)
    message(FATAL_ERROR "could not inspect Qt source archive: ${archive_error}")
endif()
foreach(required_member
        "qtbase-everywhere-src-${SDRCAL_QT_VERSION}/LICENSES/LGPL-3.0-only.txt"
        "qtbase-everywhere-src-${SDRCAL_QT_VERSION}/LICENSES/GPL-3.0-only.txt"
        "qtbase-everywhere-src-${SDRCAL_QT_VERSION}/LICENSES/GPL-2.0-only.txt"
        "qtbase-everywhere-src-${SDRCAL_QT_VERSION}/LICENSES/Qt-GPL-exception-1.0.txt"
        "qtbase-everywhere-src-${SDRCAL_QT_VERSION}/REUSE.toml")
    string(FIND "${archive_listing}" "${required_member}" member_position)
    if(member_position EQUAL -1)
        message(FATAL_ERROR "Qt source archive is missing ${required_member}")
    endif()
endforeach()

file(READ "${SDRCAL_RUNTIME_INVENTORY}" runtime_inventory)
if(NOT runtime_inventory MATCHES "Qt(6|Core|Gui|Widgets|Concurrent)")
    message(FATAL_ERROR "runtime inventory does not identify a deployed Qt component")
endif()
if(runtime_inventory MATCHES "SoapySDR|Soapy[A-Za-z0-9_-]*")
    message(FATAL_ERROR "unexpected SoapySDR or device module entered the package")
endif()

set(disposition_dir "${SDRCAL_STAGE_DIR}/share/sdrcal/license-disposition")
set(source_dir "${disposition_dir}/corresponding-source")
file(MAKE_DIRECTORY "${source_dir}" "${SDRCAL_OUTPUT_DIR}")
get_filename_component(source_name "${SDRCAL_QT_SOURCE_ARCHIVE}" NAME)
file(COPY_FILE "${SDRCAL_QT_SOURCE_ARCHIVE}" "${source_dir}/${source_name}" ONLY_IF_DIFFERENT)
file(COPY_FILE "${SDRCAL_REPLACEMENT_INSTRUCTIONS}"
    "${disposition_dir}/QT_LIBRARY_REPLACEMENT.md" ONLY_IF_DIFFERENT)
file(COPY_FILE "${SDRCAL_RUNTIME_INVENTORY}"
    "${disposition_dir}/runtime-inventory.txt" ONLY_IF_DIFFERENT)

file(WRITE "${disposition_dir}/README.md"
"# Exact binary-license disposition\n\n"
"Platform: ${SDRCAL_PLATFORM}\n\n"
"Qt version: ${SDRCAL_QT_VERSION}\n\n"
"SDR Calibration conveys Qt dynamically under LGPL-3.0-only. The complete, "
"unmodified Qt source used for this payload accompanies the binary in "
"`corresponding-source/${source_name}` (SHA-256 `${actual_source_sha256}`). "
"The archive's `LICENSES/` directory and `REUSE.toml` are the authoritative "
"file-level license and copyright inventory, including third-party code. "
"SDR Calibration made no modifications to Qt. See "
"`QT_LIBRARY_REPLACEMENT.md` for shared-library replacement instructions.\n\n"
"Project executables and documentation remain MIT licensed. SoapySDR and SDR "
"vendor modules are absent from this payload. Operating-system libraries that "
"are referenced but not copied are not conveyed by this package.\n")

file(SHA256 "${SDRCAL_RUNTIME_INVENTORY}" inventory_sha256)
file(WRITE "${SDRCAL_OUTPUT_DIR}/license-manifest.json"
"{\n"
"  \"schema_version\": 1,\n"
"  \"platform\": \"${SDRCAL_PLATFORM}\",\n"
"  \"qt_version\": \"${SDRCAL_QT_VERSION}\",\n"
"  \"qt_license\": \"LGPL-3.0-only\",\n"
"  \"qt_source_archive\": \"${source_name}\",\n"
"  \"qt_source_sha256\": \"${actual_source_sha256}\",\n"
"  \"qt_modifications\": \"none\",\n"
"  \"runtime_inventory_sha256\": \"${inventory_sha256}\",\n"
"  \"replacement_instructions\": \"share/sdrcal/license-disposition/QT_LIBRARY_REPLACEMENT.md\",\n"
"  \"soapy_sdr_conveyed\": false,\n"
"  \"gate\": \"passed\"\n"
"}\n")
file(COPY_FILE "${SDRCAL_OUTPUT_DIR}/license-manifest.json"
    "${disposition_dir}/license-manifest.json" ONLY_IF_DIFFERENT)
