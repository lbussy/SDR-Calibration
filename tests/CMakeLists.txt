add_subdirectory(unit)

add_test(NAME license_disposition_tests
    COMMAND "${CMAKE_COMMAND}" -DSDRCAL_TEST_SOURCE_DIR=${PROJECT_SOURCE_DIR}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/LicenseDispositionTest.cmake")
