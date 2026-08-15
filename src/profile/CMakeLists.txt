add_library(sdrcal_profile
    CanonicalJson.cpp
    ProfileEngine.cpp
    ProfileStorage.cpp
    Sha256.cpp
)

target_include_directories(sdrcal_profile PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_link_libraries(sdrcal_profile PUBLIC sdrcal_core)
target_compile_features(sdrcal_profile PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(sdrcal_profile PRIVATE /W4 /permissive-)
else()
    target_compile_options(sdrcal_profile PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
