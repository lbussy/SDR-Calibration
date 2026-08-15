# Production targets will be added as bounded implementation slices.
# All project-owned C++ source remains below this directory.

set(SDRCAL_SOURCE_COMPONENTS
    capture
    core
    profile
    soapy
    application
    adapters
    cli
    gui
)

foreach(component IN LISTS SDRCAL_SOURCE_COMPONENTS)
    if(component STREQUAL "soapy" AND NOT SDRCAL_ENABLE_SOAPYSDR)
        continue()
    endif()
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${component}/CMakeLists.txt")
        add_subdirectory("${component}")
    endif()
endforeach()
