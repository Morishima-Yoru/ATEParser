include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(CMAKE_TLS_VERIFY 0)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Build sources directly (avoids MinGW DLL export-table edge cases for templates).
add_executable(unit_tests
    test_main.cpp
    test_safe_conversion.cpp
    test_field_value.cpp
    test_records_analog.cpp
    test_records_digital.cpp
    test_records_shorts.cpp
    test_records_system.cpp
    test_records_coverage.cpp
    test_record_tree.cpp
    test_extended_coverage.cpp
    test_enums.cpp
    test_parser.cpp
    test_logging.cpp
    test_errors.cpp
    test_benchmark.cpp
    ${CMAKE_SOURCE_DIR}/src/enums/prefix.cpp
    ${CMAKE_SOURCE_DIR}/src/enums/test_status.cpp
    ${CMAKE_SOURCE_DIR}/src/enums/field_type.cpp
    ${CMAKE_SOURCE_DIR}/src/utils/safe_conversion.cpp
    ${CMAKE_SOURCE_DIR}/src/records/analog.cpp
    ${CMAKE_SOURCE_DIR}/src/records/digital.cpp
    ${CMAKE_SOURCE_DIR}/src/core/record.cpp
    ${CMAKE_SOURCE_DIR}/src/core/record_tree.cpp
    ${CMAKE_SOURCE_DIR}/src/core/parser.cpp
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(unit_tests
    PRIVATE
        GTest::gtest_main
        spdlog::spdlog
        cpptrace::cpptrace
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(unit_tests PRIVATE --coverage)
    target_link_options(unit_tests PRIVATE --coverage)
endif()

include(GoogleTest)
gtest_discover_tests(unit_tests)
