add_subdirectory(unit)

find_package(Python3 COMPONENTS Interpreter REQUIRED)
add_test(NAME branding_contract_tests
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_SOURCE_DIR}/scripts/verify-branding.py"
        --source-dir "${PROJECT_SOURCE_DIR}")

add_test(NAME license_disposition_tests
    COMMAND "${CMAKE_COMMAND}" -DSDRCAL_TEST_SOURCE_DIR=${PROJECT_SOURCE_DIR}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/LicenseDispositionTest.cmake")
