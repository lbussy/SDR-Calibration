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

if(SDRCAL_ENABLE_SOAPYSDR)
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

    if(MSVC)
        target_compile_options(sdrcal_capture_cli_workflow PRIVATE /W4 /permissive-)
        target_compile_options(sdrcal-capture PRIVATE /W4 /permissive-)
    else()
        target_compile_options(
            sdrcal_capture_cli_workflow PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        target_compile_options(sdrcal-capture PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()
