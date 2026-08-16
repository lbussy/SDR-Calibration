add_library(sdrcal_capture_cli_args
    CaptureCliArguments.cpp
)

target_include_directories(sdrcal_capture_cli_args PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_compile_features(sdrcal_capture_cli_args PUBLIC cxx_std_20)
target_link_libraries(sdrcal_capture_cli_args PUBLIC sdrcal_capture)

if(MSVC)
    target_compile_options(sdrcal_capture_cli_args PRIVATE /W4 /permissive-)
else()
    target_compile_options(
        sdrcal_capture_cli_args PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_library(sdrcal_product_cli ProductionCli.cpp)
target_include_directories(sdrcal_product_cli PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_compile_features(sdrcal_product_cli PUBLIC cxx_std_20)
target_link_libraries(sdrcal_product_cli PUBLIC sdrcal_application)
target_compile_definitions(sdrcal_product_cli PRIVATE
    SDRCAL_RECORDED_INPUT_MAX_BYTES=${SDRCAL_RECORDED_INPUT_MAX_BYTES})

if(SDRCAL_BUILD_CLI)
    add_executable(sdrcal product_main.cpp)
    target_compile_features(sdrcal PRIVATE cxx_std_20)
    target_link_libraries(sdrcal PRIVATE sdrcal_product_cli)
    if(SDRCAL_ENABLE_SOAPYSDR)
        target_sources(sdrcal PRIVATE LiveCliSupport.cpp)
        target_link_libraries(sdrcal PRIVATE sdrcal_soapy)
        target_compile_definitions(sdrcal PRIVATE SDRCAL_PRODUCT_LIVE_ENABLED=1)
    endif()
    install(TARGETS sdrcal RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
endif()

if(MSVC)
    target_compile_options(sdrcal_product_cli PRIVATE /W4 /permissive-)
    if(TARGET sdrcal)
        target_compile_options(sdrcal PRIVATE /W4 /permissive-)
    endif()
else()
    target_compile_options(sdrcal_product_cli PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    if(TARGET sdrcal)
        target_compile_options(sdrcal PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(SDRCAL_BUILD_CLI AND SDRCAL_ENABLE_SOAPYSDR)
    add_library(sdrcal_capture_cli_workflow
        CaptureCliWorkflow.cpp
    )
    target_include_directories(sdrcal_capture_cli_workflow PUBLIC "${PROJECT_SOURCE_DIR}/src")
    target_compile_features(sdrcal_capture_cli_workflow PUBLIC cxx_std_20)
    target_link_libraries(
        sdrcal_capture_cli_workflow PUBLIC sdrcal_capture_cli_args sdrcal_soapy)

    add_executable(sdrcal-capture main.cpp)
    target_compile_features(sdrcal-capture PRIVATE cxx_std_20)
    target_link_libraries(sdrcal-capture PRIVATE sdrcal_capture_cli_workflow)
    install(TARGETS sdrcal-capture RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

    if(MSVC)
        target_compile_options(sdrcal_capture_cli_workflow PRIVATE /W4 /permissive-)
        target_compile_options(sdrcal-capture PRIVATE /W4 /permissive-)
    else()
        target_compile_options(
            sdrcal_capture_cli_workflow PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        target_compile_options(sdrcal-capture PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()
