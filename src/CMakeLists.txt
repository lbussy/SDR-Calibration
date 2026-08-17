# Production targets will be added as bounded implementation slices.
# All project-owned C++ source remains below this directory.

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/Version.h.in"
    "${PROJECT_BINARY_DIR}/generated/sdrcal/Version.h"
    @ONLY
)
add_library(sdrcal_version INTERFACE)
target_include_directories(
    sdrcal_version INTERFACE "${PROJECT_BINARY_DIR}/generated")
target_compile_features(sdrcal_version INTERFACE cxx_std_20)

set(SDRCAL_SOURCE_COMPONENTS
    capture
    core
    profile
    reference
    evidence
    soapy
    adapters
    application
    cli
    gui
)

foreach(component IN LISTS SDRCAL_SOURCE_COMPONENTS)
    if(component STREQUAL "soapy" AND NOT SDRCAL_ENABLE_SOAPYSDR)
        continue()
    endif()
    if(component STREQUAL "cli" AND NOT SDRCAL_BUILD_CLI AND NOT SDRCAL_BUILD_GUI)
        continue()
    endif()
    if(component STREQUAL "gui" AND NOT SDRCAL_BUILD_GUI)
        continue()
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${component}/CMakeLists.txt")
        add_subdirectory("${component}")
    endif()
endforeach()
