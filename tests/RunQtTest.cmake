if(NOT DEFINED SDRCAL_TEST_EXECUTABLE OR
   NOT EXISTS "${SDRCAL_TEST_EXECUTABLE}")
    message(FATAL_ERROR "Qt test executable is missing")
endif()
if(NOT DEFINED SDRCAL_QT_RUNTIME_DIR OR
   NOT IS_DIRECTORY "${SDRCAL_QT_RUNTIME_DIR}")
    message(FATAL_ERROR "configured Qt runtime directory is missing")
endif()

if(WIN32)
    set(ENV{PATH} "${SDRCAL_QT_RUNTIME_DIR};$ENV{PATH}")
elseif(APPLE)
    set(ENV{DYLD_LIBRARY_PATH} "${SDRCAL_QT_RUNTIME_DIR}:$ENV{DYLD_LIBRARY_PATH}")
else()
    set(ENV{LD_LIBRARY_PATH} "${SDRCAL_QT_RUNTIME_DIR}:$ENV{LD_LIBRARY_PATH}")
endif()
set(ENV{QT_QPA_PLATFORM} "offscreen")

execute_process(
    COMMAND "${SDRCAL_TEST_EXECUTABLE}"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Qt test failed with exit code ${result}")
endif()
