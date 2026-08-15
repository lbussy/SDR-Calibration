add_library(sdrcal_capture
    CaptureManifestWriter.cpp
    CapturePlan.cpp
    CaptureRecorder.cpp
    RawIqWriter.cpp
)

target_include_directories(sdrcal_capture PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_compile_features(sdrcal_capture PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(sdrcal_capture PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdrcal_capture PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
