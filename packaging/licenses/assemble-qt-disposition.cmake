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

set(additional_archives ${SDRCAL_QT_ADDITIONAL_SOURCE_ARCHIVES})
set(additional_hashes ${SDRCAL_QT_ADDITIONAL_SOURCE_SHA256})
list(LENGTH additional_archives additional_archive_count)
list(LENGTH additional_hashes additional_hash_count)
if(NOT additional_archive_count EQUAL additional_hash_count)
    message(FATAL_ERROR "additional Qt source archive and SHA-256 counts differ")
endif()
if(runtime_inventory MATCHES "Qt(6)?Svg" AND additional_archive_count EQUAL 0)
    message(FATAL_ERROR "deployed QtSvg requires its matching corresponding-source archive")
endif()
if(runtime_inventory MATCHES "SoapySDR|Soapy[A-Za-z0-9_-]*")
    message(FATAL_ERROR "unexpected SoapySDR or device module entered the package")
endif()

set(disposition_dir "${SDRCAL_STAGE_DIR}/share/sdrcal/license-disposition")
set(source_dir "${disposition_dir}/corresponding-source")
file(MAKE_DIRECTORY "${source_dir}" "${SDRCAL_OUTPUT_DIR}")
get_filename_component(source_name "${SDRCAL_QT_SOURCE_ARCHIVE}" NAME)
file(COPY_FILE "${SDRCAL_QT_SOURCE_ARCHIVE}" "${source_dir}/${source_name}" ONLY_IF_DIFFERENT)
set(additional_source_manifest "")
if(additional_archive_count GREATER 0)
    math(EXPR additional_last "${additional_archive_count} - 1")
    foreach(index RANGE ${additional_last})
        list(GET additional_archives ${index} additional_archive)
        list(GET additional_hashes ${index} additional_expected_hash)
        if(NOT EXISTS "${additional_archive}")
            message(FATAL_ERROR "additional Qt source archive does not exist: ${additional_archive}")
        endif()
        string(LENGTH "${additional_expected_hash}" additional_hash_length)
        if(NOT additional_hash_length EQUAL 64 OR
           NOT additional_expected_hash MATCHES "^[0-9a-fA-F]+$")
            message(FATAL_ERROR "additional Qt source SHA-256 is invalid")
        endif()
        file(SHA256 "${additional_archive}" additional_actual_hash)
        string(TOLOWER "${additional_expected_hash}" additional_expected_hash)
        if(NOT additional_actual_hash STREQUAL additional_expected_hash)
            message(FATAL_ERROR "additional Qt source SHA-256 mismatch: ${additional_archive}")
        endif()
        execute_process(COMMAND "${CMAKE_COMMAND}" -E tar tf "${additional_archive}"
            RESULT_VARIABLE additional_list_result OUTPUT_VARIABLE additional_listing
            ERROR_VARIABLE additional_list_error)
        if(NOT additional_list_result EQUAL 0 OR
           NOT additional_listing MATCHES "qtsvg-everywhere-src-${SDRCAL_QT_VERSION}/")
            message(FATAL_ERROR "additional Qt source is not matching qtsvg: ${additional_list_error}")
        endif()
        foreach(required_additional_member
                "qtsvg-everywhere-src-${SDRCAL_QT_VERSION}/LICENSES/LGPL-3.0-only.txt"
                "qtsvg-everywhere-src-${SDRCAL_QT_VERSION}/REUSE.toml")
            string(FIND "${additional_listing}" "${required_additional_member}" additional_position)
            if(additional_position EQUAL -1)
                message(FATAL_ERROR "additional Qt source is missing ${required_additional_member}")
            endif()
        endforeach()
        get_filename_component(additional_name "${additional_archive}" NAME)
        file(COPY_FILE "${additional_archive}" "${source_dir}/${additional_name}"
            ONLY_IF_DIFFERENT)
        if(NOT additional_source_manifest STREQUAL "")
            string(APPEND additional_source_manifest ",")
        endif()
        string(APPEND additional_source_manifest
            "\n    {\"name\": \"${additional_name}\", \"sha256\": \"${additional_actual_hash}\"}")
    endforeach()
endif()
file(COPY_FILE "${SDRCAL_REPLACEMENT_INSTRUCTIONS}"
    "${disposition_dir}/QT_LIBRARY_REPLACEMENT.md" ONLY_IF_DIFFERENT)
file(COPY_FILE "${SDRCAL_RUNTIME_INVENTORY}"
    "${disposition_dir}/runtime-inventory.txt" ONLY_IF_DIFFERENT)

file(WRITE "${disposition_dir}/README.md"
"# Exact binary-license disposition\n\n"
"Platform: ${SDRCAL_PLATFORM}\n\n"
"Qt version: ${SDRCAL_QT_VERSION}\n\n"
"SDR Calibration conveys Qt dynamically under LGPL-3.0-only. The complete, "
"unmodified Qt source used for this payload accompanies the binary in the "
"`corresponding-source/` directory. The exact archive names and hashes are "
"recorded in `license-manifest.json`. Each archive's `LICENSES/` directory "
"and `REUSE.toml` are the authoritative "
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
"  \"additional_qt_sources\": [${additional_source_manifest}\n  ],\n"
"  \"qt_modifications\": \"none\",\n"
"  \"runtime_inventory_sha256\": \"${inventory_sha256}\",\n"
"  \"replacement_instructions\": \"share/sdrcal/license-disposition/QT_LIBRARY_REPLACEMENT.md\",\n"
"  \"soapy_sdr_conveyed\": false,\n"
"  \"gate\": \"passed\"\n"
"}\n")
file(COPY_FILE "${SDRCAL_OUTPUT_DIR}/license-manifest.json"
    "${disposition_dir}/license-manifest.json" ONLY_IF_DIFFERENT)
