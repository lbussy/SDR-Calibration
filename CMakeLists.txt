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

option(SDRCAL_BUILD_CLI "Build the command-line application" ON)
option(SDRCAL_BUILD_GUI "Build the Qt 6 Widgets application" ON)
option(SDRCAL_ENABLE_SOAPYSDR "Enable SoapySDR device support" ON)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
