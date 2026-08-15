cmake_minimum_required(VERSION 3.20)

set(root "${CMAKE_CURRENT_BINARY_DIR}/license-disposition-test")
file(REMOVE_RECURSE "${root}")
set(source_root "${root}/fixture/qtbase-everywhere-src-6.2.0")
file(MAKE_DIRECTORY "${source_root}/LICENSES" "${root}/stage" "${root}/evidence")
foreach(license LGPL-3.0-only.txt GPL-3.0-only.txt GPL-2.0-only.txt
        Qt-GPL-exception-1.0.txt)
    file(WRITE "${source_root}/LICENSES/${license}" "synthetic test license ${license}\n")
endforeach()
file(WRITE "${source_root}/REUSE.toml" "version = 1\n")
set(archive "${root}/qtbase-everywhere-src-6.2.0.tar")
execute_process(COMMAND "${CMAKE_COMMAND}" -E tar cf "${archive}"
    "qtbase-everywhere-src-6.2.0" WORKING_DIRECTORY "${root}/fixture"
    RESULT_VARIABLE archive_result)
if(NOT archive_result EQUAL 0)
    message(FATAL_ERROR "could not create synthetic Qt source archive")
endif()
file(SHA256 "${archive}" archive_sha256)
file(WRITE "${root}/runtime.txt" "FILE Qt6Core.dll\nFILE Qt6Widgets.dll\n")
set(common_args
    -DSDRCAL_STAGE_DIR=${root}/stage
    -DSDRCAL_OUTPUT_DIR=${root}/evidence
    -DSDRCAL_PLATFORM=Windows
    -DSDRCAL_QT_VERSION=6.2.0
    -DSDRCAL_QT_SOURCE_ARCHIVE=${archive}
    -DSDRCAL_RUNTIME_INVENTORY=${root}/runtime.txt
    -DSDRCAL_REPLACEMENT_INSTRUCTIONS=${SDRCAL_TEST_SOURCE_DIR}/packaging/licenses/qt-library-replacement.md
    -P ${SDRCAL_TEST_SOURCE_DIR}/packaging/licenses/assemble-qt-disposition.cmake)
execute_process(COMMAND "${CMAKE_COMMAND}" -DSDRCAL_QT_SOURCE_SHA256=${archive_sha256}
    ${common_args} RESULT_VARIABLE pass_result)
if(NOT pass_result EQUAL 0 OR
   NOT EXISTS "${root}/stage/share/sdrcal/license-disposition/license-manifest.json")
    message(FATAL_ERROR "valid synthetic disposition did not pass")
endif()
file(READ "${root}/evidence/license-manifest.json" license_manifest)
if(NOT license_manifest MATCHES "\"additional_qt_sources\": \\[")
    message(FATAL_ERROR "license manifest does not record additional Qt sources")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}"
    -DSDRCAL_QT_SOURCE_SHA256=aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
    ${common_args} RESULT_VARIABLE mismatch_result OUTPUT_QUIET ERROR_QUIET)
if(mismatch_result EQUAL 0)
    message(FATAL_ERROR "source hash mismatch did not fail closed")
endif()
file(WRITE "${root}/runtime.txt" "FILE Qt6Core.dll\nFILE SoapySDR.dll\n")
execute_process(COMMAND "${CMAKE_COMMAND}" -DSDRCAL_QT_SOURCE_SHA256=${archive_sha256}
    ${common_args} RESULT_VARIABLE soapy_result OUTPUT_QUIET ERROR_QUIET)
if(soapy_result EQUAL 0)
    message(FATAL_ERROR "unexpected SoapySDR payload did not fail closed")
endif()
file(WRITE "${root}/runtime.txt" "FILE Qt6Core.dll\nFILE Qt6Svg.dll\n")
execute_process(COMMAND "${CMAKE_COMMAND}" -DSDRCAL_QT_SOURCE_SHA256=${archive_sha256}
    ${common_args} RESULT_VARIABLE svg_without_source_result
    OUTPUT_QUIET ERROR_QUIET)
if(svg_without_source_result EQUAL 0)
    message(FATAL_ERROR "QtSvg payload without qtsvg source did not fail closed")
endif()
set(svg_source_root "${root}/fixture/qtsvg-everywhere-src-6.2.0")
file(MAKE_DIRECTORY "${svg_source_root}/LICENSES")
file(WRITE "${svg_source_root}/LICENSES/LGPL-3.0-only.txt" "synthetic LGPL\n")
file(WRITE "${svg_source_root}/REUSE.toml" "version = 1\n")
set(svg_archive "${root}/qtsvg-everywhere-src-6.2.0.tar")
execute_process(COMMAND "${CMAKE_COMMAND}" -E tar cf "${svg_archive}"
    "qtsvg-everywhere-src-6.2.0" WORKING_DIRECTORY "${root}/fixture"
    RESULT_VARIABLE svg_archive_result)
if(NOT svg_archive_result EQUAL 0)
    message(FATAL_ERROR "could not create synthetic QtSvg source archive")
endif()
file(SHA256 "${svg_archive}" svg_archive_sha256)
execute_process(COMMAND "${CMAKE_COMMAND}"
    -DSDRCAL_QT_SOURCE_SHA256=${archive_sha256}
    -DSDRCAL_QT_ADDITIONAL_SOURCE_ARCHIVES=${svg_archive}
    -DSDRCAL_QT_ADDITIONAL_SOURCE_SHA256=${svg_archive_sha256}
    ${common_args} RESULT_VARIABLE svg_with_source_result)
if(NOT svg_with_source_result EQUAL 0 OR
   NOT EXISTS "${root}/stage/share/sdrcal/license-disposition/corresponding-source/qtsvg-everywhere-src-6.2.0.tar")
    message(FATAL_ERROR "QtSvg payload with matching qtsvg source did not pass")
endif()
