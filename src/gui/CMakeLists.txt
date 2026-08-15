find_package(Qt6 ${SDRCAL_MINIMUM_QT_VERSION} QUIET COMPONENTS Core Widgets Concurrent)

if(NOT Qt6_FOUND)
    if(SDRCAL_REQUIRE_GUI)
        message(FATAL_ERROR
            "Qt ${SDRCAL_MINIMUM_QT_VERSION}+ Core, Widgets, and Concurrent are required")
    endif()
    message(STATUS "Qt 6 Core, Widgets, and Concurrent not found; sdrcal-gui is disabled")
    return()
endif()

set_property(GLOBAL PROPERTY SDRCAL_CONFIGURED_QT_VERSION "${Qt6_VERSION}")

if(WIN32)
    enable_language(RC)
    file(TO_CMAKE_PATH
        "${PROJECT_SOURCE_DIR}/assets/icons/windows/SDRCalibration.ico"
        SDRCAL_WINDOWS_ICON_FILE)
    configure_file(
        "${PROJECT_SOURCE_DIR}/packaging/windows/sdrcal.rc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/sdrcal.rc"
        @ONLY)
endif()

add_library(sdrcal_gui_support GuiWorkflow.cpp MainWindow.cpp)
target_include_directories(sdrcal_gui_support PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_compile_features(sdrcal_gui_support PUBLIC cxx_std_20)
target_link_libraries(sdrcal_gui_support PUBLIC sdrcal_product_cli Qt6::Core Qt6::Widgets Qt6::Concurrent)

add_executable(sdrcal-gui WIN32 MACOSX_BUNDLE main.cpp)
target_compile_features(sdrcal-gui PRIVATE cxx_std_20)
target_link_libraries(sdrcal-gui PRIVATE sdrcal_gui_support)

if(APPLE)
    set(SDRCAL_MACOS_ICON
        "${PROJECT_SOURCE_DIR}/assets/icons/macos/SDRCalibration.icns")
    target_sources(sdrcal-gui PRIVATE "${SDRCAL_MACOS_ICON}")
    set_source_files_properties("${SDRCAL_MACOS_ICON}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    set_target_properties(sdrcal-gui PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "SDRCalibration.icns"
        MACOSX_BUNDLE_INFO_PLIST "${PROJECT_SOURCE_DIR}/packaging/macos/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "SDR Calibration"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_COPYRIGHT "Copyright SDR Calibration contributors"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.lbussy.sdrcal"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
    )
endif()

if(WIN32)
    target_sources(sdrcal-gui PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/sdrcal.rc")
endif()

if(APPLE)
    # Keep the build target and executable basename stable while giving the
    # installed bundle its product-facing Finder name.
    install(DIRECTORY "$<TARGET_BUNDLE_DIR:sdrcal-gui>/"
        DESTINATION "SDR Calibration.app"
        USE_SOURCE_PERMISSIONS)
else()
    install(TARGETS sdrcal-gui RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

if(MSVC)
    target_compile_options(sdrcal_gui_support PRIVATE /W4 /permissive-)
    target_compile_options(sdrcal-gui PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdrcal_gui_support PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    target_compile_options(sdrcal-gui PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
