add_executable(capture_unit_tests CaptureTests.cpp)
target_link_libraries(capture_unit_tests PRIVATE sdrcal_capture)
target_compile_features(capture_unit_tests PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(capture_unit_tests PRIVATE /W4 /permissive-)
else()
    target_compile_options(capture_unit_tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

add_test(NAME capture_unit_tests COMMAND capture_unit_tests)

if(TARGET sdrcal_soapy)
    add_executable(soapy_adapter_unit_tests SoapyAdapterTests.cpp)
    target_link_libraries(soapy_adapter_unit_tests PRIVATE sdrcal_soapy)
    target_compile_features(soapy_adapter_unit_tests PRIVATE cxx_std_20)
    if(MSVC)
        target_compile_options(soapy_adapter_unit_tests PRIVATE /W4 /permissive-)
    else()
        target_compile_options(
            soapy_adapter_unit_tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
    add_test(NAME soapy_adapter_unit_tests COMMAND soapy_adapter_unit_tests)
endif()
