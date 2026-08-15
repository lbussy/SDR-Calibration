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

add_library(sdrcal_gui_support GuiWorkflow.cpp MainWindow.cpp)
target_include_directories(sdrcal_gui_support PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_compile_features(sdrcal_gui_support PUBLIC cxx_std_20)
target_link_libraries(sdrcal_gui_support PUBLIC sdrcal_product_cli Qt6::Core Qt6::Widgets Qt6::Concurrent)

add_executable(sdrcal-gui WIN32 MACOSX_BUNDLE main.cpp)
target_compile_features(sdrcal-gui PRIVATE cxx_std_20)
target_link_libraries(sdrcal-gui PRIVATE sdrcal_gui_support)

install(TARGETS sdrcal-gui RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    BUNDLE DESTINATION ".")

if(MSVC)
    target_compile_options(sdrcal_gui_support PRIVATE /W4 /permissive-)
    target_compile_options(sdrcal-gui PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdrcal_gui_support PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    target_compile_options(sdrcal-gui PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
