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
