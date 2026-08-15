cmake_minimum_required(VERSION 3.20)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not supported. Use a CMake preset, for example: "
        "cmake --preset default && cmake --build --preset default"
    )
endif()

project(
    SDRCalibration
    VERSION 0.1.0
    DESCRIPTION "Cross-platform SDR frequency calibration"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(GNUInstallDirs)

option(SDRCAL_BUILD_CLI "Build the command-line application" ON)
option(SDRCAL_BUILD_GUI "Build the Qt 6 Widgets application" ON)
option(SDRCAL_ENABLE_SOAPYSDR "Enable SoapySDR device support" ON)
option(SDRCAL_REQUIRE_GUI "Fail configuration when the requested GUI cannot be built" OFF)
option(SDRCAL_ENABLE_PACKAGING "Enable installable application package targets" ON)

set(SDRCAL_MINIMUM_QT_VERSION "6.2")
set(SDRCAL_MINIMUM_SOAPYSDR_VERSION "0.8")

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(FILES LICENSE THIRD_PARTY_NOTICES.md DESTINATION "${CMAKE_INSTALL_DATADIR}/sdrcal")

get_property(SDRCAL_CONFIGURED_QT_VERSION GLOBAL PROPERTY SDRCAL_CONFIGURED_QT_VERSION)
get_property(
    SDRCAL_CONFIGURED_SOAPYSDR_VERSION GLOBAL PROPERTY SDRCAL_CONFIGURED_SOAPYSDR_VERSION)
set(SDRCAL_SPDX_DEPENDENCY_PACKAGES "")
set(SDRCAL_SPDX_RELATIONSHIPS "")
if(SDRCAL_CONFIGURED_QT_VERSION)
    string(APPEND SDRCAL_SPDX_DEPENDENCY_PACKAGES
        ",\n    {\n      \"name\": \"Qt\",\n      \"SPDXID\": \"SPDXRef-Package-Qt\",\n"
        "      \"versionInfo\": \"${SDRCAL_CONFIGURED_QT_VERSION}\",\n"
        "      \"downloadLocation\": \"https://www.qt.io/download-open-source\",\n"
        "      \"filesAnalyzed\": false,\n      \"licenseConcluded\": \"NOASSERTION\",\n"
        "      \"licenseDeclared\": \"NOASSERTION\",\n"
        "      \"copyrightText\": \"NOASSERTION\"\n    }")
    string(APPEND SDRCAL_SPDX_RELATIONSHIPS
        ",\n    {\"spdxElementId\": \"SPDXRef-Package-SDRCalibration\","
        " \"relationshipType\": \"DEPENDS_ON\","
        " \"relatedSpdxElement\": \"SPDXRef-Package-Qt\"}")
endif()
if(SDRCAL_CONFIGURED_SOAPYSDR_VERSION)
    string(APPEND SDRCAL_SPDX_DEPENDENCY_PACKAGES
        ",\n    {\n      \"name\": \"SoapySDR\",\n"
        "      \"SPDXID\": \"SPDXRef-Package-SoapySDR\",\n"
        "      \"versionInfo\": \"${SDRCAL_CONFIGURED_SOAPYSDR_VERSION}\",\n"
        "      \"downloadLocation\": \"https://github.com/pothosware/SoapySDR\",\n"
        "      \"filesAnalyzed\": false,\n      \"licenseConcluded\": \"NOASSERTION\",\n"
        "      \"licenseDeclared\": \"NOASSERTION\",\n"
        "      \"copyrightText\": \"NOASSERTION\"\n    }")
    string(APPEND SDRCAL_SPDX_RELATIONSHIPS
        ",\n    {\"spdxElementId\": \"SPDXRef-Package-SDRCalibration\","
        " \"relationshipType\": \"DEPENDS_ON\","
        " \"relatedSpdxElement\": \"SPDXRef-Package-SoapySDR\"}")
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/packaging/sdrcal.spdx.json.in"
    "${PROJECT_BINARY_DIR}/sdrcal.spdx.json"
    @ONLY
)
install(FILES "${PROJECT_BINARY_DIR}/sdrcal.spdx.json"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/sdrcal")

if(SDRCAL_ENABLE_PACKAGING AND NOT TARGET sdrcal)
    message(FATAL_ERROR "SDRCAL_ENABLE_PACKAGING requires SDRCAL_BUILD_CLI=ON")
endif()

set(SDRCAL_PACKAGE_AUDIT_DEPENDENCIES sdrcal)
if(TARGET sdrcal-capture)
    list(APPEND SDRCAL_PACKAGE_AUDIT_DEPENDENCIES sdrcal-capture)
endif()
if(TARGET sdrcal-gui)
    list(APPEND SDRCAL_PACKAGE_AUDIT_DEPENDENCIES sdrcal-gui)
endif()
if(SDRCAL_ENABLE_PACKAGING)
add_custom_target(package-audit
    COMMAND "${CMAKE_COMMAND}"
        -DSDRCAL_BUILD_DIR=${PROJECT_BINARY_DIR}
        -DSDRCAL_STAGE_DIR=${PROJECT_BINARY_DIR}/package-audit-root
        -DSDRCAL_BINDIR=${CMAKE_INSTALL_BINDIR}
        -DSDRCAL_DATADIR=${CMAKE_INSTALL_DATADIR}
        -DSDRCAL_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}
        -DSDRCAL_GUI_IS_BUNDLE=${APPLE}
        -DSDRCAL_EXPECT_CAPTURE=${SDRCAL_ENABLE_SOAPYSDR}
        -DSDRCAL_EXPECT_GUI=${SDRCAL_BUILD_GUI}
        -P "${PROJECT_SOURCE_DIR}/packaging/audit-package.cmake"
    DEPENDS ${SDRCAL_PACKAGE_AUDIT_DEPENDENCIES}
    VERBATIM
)
endif()

set(CPACK_PACKAGE_NAME "SDRCalibration")
set(CPACK_PACKAGE_VENDOR "SDR Calibration contributors")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Traceable per-device SDR frequency calibration")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(SDRCAL_BINARY_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_FILE_NAME "${SDRCAL_BINARY_PACKAGE_FILE_NAME}")
if(WIN32)
    set(CPACK_GENERATOR ZIP)
else()
    set(CPACK_GENERATOR TGZ)
endif()
if(SDRCAL_ENABLE_PACKAGING)
    include(CPack)
    if(WIN32)
        set(SDRCAL_PACKAGE_ARCHIVE
            "${PROJECT_BINARY_DIR}/${SDRCAL_BINARY_PACKAGE_FILE_NAME}.zip")
    else()
        set(SDRCAL_PACKAGE_ARCHIVE
            "${PROJECT_BINARY_DIR}/${SDRCAL_BINARY_PACKAGE_FILE_NAME}.tar.gz")
    endif()
    add_custom_target(package-archive-audit
        COMMAND "${CMAKE_CPACK_COMMAND}" --config "${PROJECT_BINARY_DIR}/CPackConfig.cmake"
            -C $<CONFIG>
        COMMAND "${CMAKE_COMMAND}"
            -DSDRCAL_ARCHIVE=${SDRCAL_PACKAGE_ARCHIVE}
            -DSDRCAL_AUDIT_ROOT=${PROJECT_BINARY_DIR}/package-archive-audit-root
            -DSDRCAL_EXPECT_CAPTURE=${SDRCAL_ENABLE_SOAPYSDR}
            -DSDRCAL_EXPECT_GUI=${SDRCAL_BUILD_GUI}
            -DSDRCAL_EXECUTABLE_SUFFIX=${CMAKE_EXECUTABLE_SUFFIX}
            -P "${PROJECT_SOURCE_DIR}/packaging/audit-archive.cmake"
        DEPENDS ${SDRCAL_PACKAGE_AUDIT_DEPENDENCIES}
        VERBATIM
    )
endif()
