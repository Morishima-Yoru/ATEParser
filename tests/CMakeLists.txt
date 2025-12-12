include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  unit_tests
  test_main.cpp
  test_parser.cpp
  test_records_analog.cpp
  test_records_system.cpp
  test_records_digital.cpp
  test_records_shorts.cpp
  test_edge_cases.cpp
  test_core_utils.cpp
  ../src/containers/LogRecordContainer.cpp
  ../src/containers/LogRecordFactory.cpp
  ../src/core/FieldValue.cpp
  ../src/core/I3070LogParser.cpp
  ../src/core/LogRecord.cpp
  ${CMAKE_BINARY_DIR}/src/core/Version.cpp
  ../src/enums/FieldType.cpp
  ../src/enums/LogRecordPrefix.cpp
  ../src/enums/TestStatus.cpp
  ../src/records/AnalogRecords.cpp
  ../src/records/DigitalRecords.cpp
  ../src/records/ShortRecords.cpp
  ../src/records/SystemRecords.cpp
  ../src/utils/SafeConversion.cpp
)

target_include_directories(unit_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(
  unit_tests
  GTest::gtest_main
)

# Enable coverage
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(unit_tests PRIVATE --coverage)
    target_link_options(unit_tests PRIVATE --coverage)
endif()

add_dependencies(unit_tests UpdateVersion)

include(GoogleTest)
gtest_discover_tests(unit_tests)
