find_package(SoapySDR CONFIG REQUIRED)

add_library(sdrcal_soapy
    RealSoapyApi.cpp
    SoapyCaptureSession.cpp
    SoapyReadTranslator.cpp
)

target_include_directories(sdrcal_soapy PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_compile_features(sdrcal_soapy PUBLIC cxx_std_20)
target_link_libraries(sdrcal_soapy PUBLIC sdrcal_capture PRIVATE SoapySDR)

if(MSVC)
    target_compile_options(sdrcal_soapy PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdrcal_soapy PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

message(STATUS "SoapySDR receive adapter enabled")
