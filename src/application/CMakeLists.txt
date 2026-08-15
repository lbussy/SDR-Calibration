add_library(sdrcal_application CalibrationWorkflow.cpp)
target_include_directories(sdrcal_application PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(
    sdrcal_application
    PUBLIC sdrcal_core sdrcal_profile sdrcal_reference sdrcal_evidence sdrcal_wsjtx)
target_compile_features(sdrcal_application PUBLIC cxx_std_20)
if(MSVC)
    target_compile_options(sdrcal_application PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdrcal_application PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
