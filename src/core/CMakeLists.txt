add_library(sdrcal_core
    CalibrationModel.cpp
    CarrierEstimator.cpp
    ObservationAcceptance.cpp
    ReliabilityQuotient.cpp
    SignalQualityAnalyzer.cpp
    Uncertainty.cpp
)

target_include_directories(sdrcal_core PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_compile_features(sdrcal_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(sdrcal_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdrcal_core PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
