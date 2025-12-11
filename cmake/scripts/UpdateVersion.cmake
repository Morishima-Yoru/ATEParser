# UpdateVersion.cmake

# Read Version (Major.Minor.Patch)
file(READ "${SOURCE_DIR}/VERSION" PROJECT_VERSION_RAW)
string(STRIP "${PROJECT_VERSION_RAW}" PROJECT_VERSION_RAW)
string(REPLACE "." ";" VERSION_LIST "${PROJECT_VERSION_RAW}")
list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

# Read and Increment Build Number
if(EXISTS "${SOURCE_DIR}/cmake/BUILD_NUMBER")
    file(READ "${SOURCE_DIR}/cmake/BUILD_NUMBER" PROJECT_BUILD_NUMBER)
    string(STRIP "${PROJECT_BUILD_NUMBER}" PROJECT_BUILD_NUMBER)
else()
    set(PROJECT_BUILD_NUMBER 0)
endif()

math(EXPR PROJECT_BUILD_NUMBER "${PROJECT_BUILD_NUMBER} + 1")
file(WRITE "${SOURCE_DIR}/cmake/BUILD_NUMBER" "${PROJECT_BUILD_NUMBER}")

# Get Git Hash
find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY "${SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_HASH "unknown")
endif()

if("${GIT_HASH}" STREQUAL "")
    set(GIT_HASH "unknown")
endif()

set(PROJECT_VERSION_FULL "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_BUILD_NUMBER}.${GIT_HASH}")

message(STATUS "Updating Version to: ${PROJECT_VERSION_FULL}")

# Configure Version.cpp
configure_file(
    "${SOURCE_DIR}/cmake/templates/Version.cpp.in"
    "${BINARY_DIR}/src/core/Version.cpp"
    @ONLY
)

# Configure Version.hpp (Less frequent changes, but ensuring consistency)
configure_file(
    "${SOURCE_DIR}/cmake/templates/Version.hpp.in"
    "${BINARY_DIR}/include/keysight_log/core/Version.hpp"
    @ONLY
)
