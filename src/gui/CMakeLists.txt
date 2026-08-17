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
get_filename_component(SDRCAL_CONFIGURED_QT_PREFIX "${Qt6_DIR}/../../.." ABSOLUTE)
set_property(GLOBAL PROPERTY SDRCAL_CONFIGURED_QT_RUNTIME_DIR
    "${SDRCAL_CONFIGURED_QT_PREFIX}/bin")

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
if(SDRCAL_ENABLE_SOAPYSDR)
    target_sources(sdrcal_gui_support PRIVATE ../cli/LiveCliSupport.cpp)
    target_link_libraries(sdrcal_gui_support PUBLIC sdrcal_soapy)
    target_compile_definitions(sdrcal_gui_support PUBLIC SDRCAL_GUI_LIVE_ENABLED=1)
endif()

add_executable(sdrcal-gui WIN32 MACOSX_BUNDLE main.cpp)
target_compile_features(sdrcal-gui PRIVATE cxx_std_20)
target_link_libraries(sdrcal-gui PRIVATE sdrcal_gui_support sdrcal_version)

if(APPLE)
    find_program(SDRCAL_XCRUN_EXECUTABLE xcrun REQUIRED)
    set(SDRCAL_MACOS_ICON_DOCUMENT
        "${PROJECT_SOURCE_DIR}/assets/icons/macos/SDRCalibration.icon")
    set(SDRCAL_MACOS_ICON_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/macos-icon")
    set(SDRCAL_MACOS_ASSET_CATALOG
        "${SDRCAL_MACOS_ICON_OUTPUT_DIR}/Assets.car")
    set(SDRCAL_MACOS_ICON
        "${SDRCAL_MACOS_ICON_OUTPUT_DIR}/SDRCalibration.icns")
    set(SDRCAL_MACOS_ICON_PARTIAL_PLIST
        "${SDRCAL_MACOS_ICON_OUTPUT_DIR}/partial.plist")
    if(CMAKE_OSX_DEPLOYMENT_TARGET)
        set(SDRCAL_MACOS_ICON_DEPLOYMENT_TARGET
            "${CMAKE_OSX_DEPLOYMENT_TARGET}")
    else()
        set(SDRCAL_MACOS_ICON_DEPLOYMENT_TARGET "14.0")
    endif()
    add_custom_command(
        OUTPUT
            "${SDRCAL_MACOS_ASSET_CATALOG}"
            "${SDRCAL_MACOS_ICON}"
            "${SDRCAL_MACOS_ICON_PARTIAL_PLIST}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory
            "${SDRCAL_MACOS_ICON_OUTPUT_DIR}"
        COMMAND "${SDRCAL_XCRUN_EXECUTABLE}" actool
            --compile "${SDRCAL_MACOS_ICON_OUTPUT_DIR}"
            --platform macosx
            --target-device mac
            --minimum-deployment-target "${SDRCAL_MACOS_ICON_DEPLOYMENT_TARGET}"
            --app-icon SDRCalibration
            --output-partial-info-plist "${SDRCAL_MACOS_ICON_PARTIAL_PLIST}"
            --standalone-icon-behavior all
            --product-type com.apple.product-type.application
            --warnings --errors --notices
            "${SDRCAL_MACOS_ICON_DOCUMENT}"
        DEPENDS
            "${SDRCAL_MACOS_ICON_DOCUMENT}/icon.json"
            "${SDRCAL_MACOS_ICON_DOCUMENT}/Assets/SDRCalibration-macOS26-1024.png"
        VERBATIM
        COMMENT "Compiling the SDR Calibration Icon Composer document")
    target_sources(sdrcal-gui PRIVATE
        "${SDRCAL_MACOS_ASSET_CATALOG}"
        "${SDRCAL_MACOS_ICON}")
    set_source_files_properties(
        "${SDRCAL_MACOS_ASSET_CATALOG}"
        "${SDRCAL_MACOS_ICON}"
        PROPERTIES GENERATED TRUE
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
    target_link_options(sdrcal-gui PRIVATE
        "/MANIFEST:EMBED"
        "/MANIFESTINPUT:${PROJECT_SOURCE_DIR}/packaging/windows/sdrcal.exe.manifest")
else()
    target_compile_options(sdrcal_gui_support PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    target_compile_options(sdrcal-gui PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
