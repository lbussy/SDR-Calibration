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

set(SDRCAL_MACOS_SIGNING_IDENTITY "" CACHE STRING
    "Developer ID Application identity used by the notarized macOS package")
set(SDRCAL_MACOS_NOTARY_PROFILE "" CACHE STRING
    "notarytool Keychain profile used by the notarized macOS package")
set(SDRCAL_WINDOWS_CERTIFICATE_THUMBPRINT "" CACHE STRING
    "SHA-1 certificate thumbprint used by the signed Windows package")
set(SDRCAL_WINDOWS_TIMESTAMP_URL "" CACHE STRING
    "RFC 3161 timestamp service used by the signed Windows package")
set(SDRCAL_QT_SOURCE_ARCHIVE "" CACHE FILEPATH
    "Complete qtbase source archive matching the Qt binaries conveyed by a release")
set(SDRCAL_QT_SOURCE_SHA256 "" CACHE STRING
    "Pinned SHA-256 of SDRCAL_QT_SOURCE_ARCHIVE")

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

if(APPLE AND SDRCAL_ENABLE_PACKAGING AND TARGET sdrcal-gui)
    set(SDRCAL_MACOS_DMG_OUTPUT_DIR "${PROJECT_BINARY_DIR}/macos-package")
    add_custom_target(macos-dmg
        COMMAND "${PROJECT_SOURCE_DIR}/packaging/macos/package-dmg.sh"
            --build-dir "${PROJECT_BINARY_DIR}"
            --output-dir "${SDRCAL_MACOS_DMG_OUTPUT_DIR}"
            --signing-identity "${SDRCAL_MACOS_SIGNING_IDENTITY}"
            --notary-profile "${SDRCAL_MACOS_NOTARY_PROFILE}"
            --version "${PROJECT_VERSION}"
            --source-dir "${PROJECT_SOURCE_DIR}"
            --qt-source-archive "${SDRCAL_QT_SOURCE_ARCHIVE}"
            --qt-source-sha256 "${SDRCAL_QT_SOURCE_SHA256}"
        DEPENDS ${SDRCAL_PACKAGE_AUDIT_DEPENDENCIES}
        VERBATIM
    )
endif()

if(WIN32 AND SDRCAL_ENABLE_PACKAGING AND TARGET sdrcal-gui)
    find_program(SDRCAL_POWERSHELL_EXECUTABLE NAMES pwsh powershell REQUIRED)
    set(SDRCAL_WINDOWS_MSI_OUTPUT_DIR "${PROJECT_BINARY_DIR}/windows-package")
    add_custom_target(windows-msi
        COMMAND "${SDRCAL_POWERSHELL_EXECUTABLE}" -NoProfile -NonInteractive
            -ExecutionPolicy Bypass
            -File "${PROJECT_SOURCE_DIR}/packaging/windows/package-msi.ps1"
            -BuildDir "${PROJECT_BINARY_DIR}"
            -OutputDir "${SDRCAL_WINDOWS_MSI_OUTPUT_DIR}"
            -CertificateThumbprint "${SDRCAL_WINDOWS_CERTIFICATE_THUMBPRINT}"
            -TimestampUrl "${SDRCAL_WINDOWS_TIMESTAMP_URL}"
            -Version "${PROJECT_VERSION}"
            -SourceDir "${PROJECT_SOURCE_DIR}"
            -QtSourceArchive "${SDRCAL_QT_SOURCE_ARCHIVE}"
            -QtSourceSha256 "${SDRCAL_QT_SOURCE_SHA256}"
        DEPENDS ${SDRCAL_PACKAGE_AUDIT_DEPENDENCIES}
        VERBATIM
    )
endif()

if(UNIX AND NOT APPLE AND SDRCAL_ENABLE_PACKAGING AND TARGET sdrcal-gui)
    install(FILES "${PROJECT_SOURCE_DIR}/packaging/ubuntu/sdrcal.desktop"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications")
    set(SDRCAL_UBUNTU_DEB_OUTPUT_DIR "${PROJECT_BINARY_DIR}/ubuntu-package")
    add_custom_target(ubuntu-deb
        COMMAND "${PROJECT_SOURCE_DIR}/packaging/ubuntu/package-deb.sh"
            --build-dir "${PROJECT_BINARY_DIR}"
            --output-dir "${SDRCAL_UBUNTU_DEB_OUTPUT_DIR}"
            --version "${PROJECT_VERSION}"
            --source-dir "${PROJECT_SOURCE_DIR}"
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
