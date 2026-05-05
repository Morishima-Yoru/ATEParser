cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    # Only force the MinGW cross-compiler when the caller has NOT already
    # specified one via -DCMAKE_CXX_COMPILER=... (e.g., CI or MSYS2 shell).
    if(NOT DEFINED CACHE{CMAKE_CXX_COMPILER})
        set(CMAKE_C_COMPILER   "x86_64-w64-mingw32-gcc.exe")
        set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++.exe")
    endif()

    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++-6.dll
        OUTPUT_VARIABLE MINGW_STDCPP_DLL
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(EXISTS "${MINGW_STDCPP_DLL}")
        get_filename_component(MINGW_BIN_DIR "${MINGW_STDCPP_DLL}" DIRECTORY)
    else()
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    endif()
    message(STATUS "MINGW_BIN_DIR: ${MINGW_BIN_DIR}")

    add_link_options(-static -static-libstdc++ -static-libgcc)
endif()

project(ATEParser LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
elseif(MSVC)
    add_compile_options(/W4 /permissive-)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

set(SPDLOG_FMT_EXTERNAL  OFF CACHE BOOL "" FORCE)
set(SPDLOG_BUILD_SHARED  OFF CACHE BOOL "" FORCE)
set(SPDLOG_INSTALL       OFF CACHE BOOL "" FORCE)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.0
    GIT_SHALLOW    TRUE
)

set(CPPTRACE_STATIC                    ON  CACHE BOOL "" FORCE)
set(CPPTRACE_BUILD_SHARED              OFF CACHE BOOL "" FORCE)
set(CPPTRACE_GET_SYMBOLS_WITH_LIBDWARF OFF CACHE BOOL "" FORCE)
set(CPPTRACE_USE_EXTERNAL_ZSTD         OFF CACHE BOOL "" FORCE)
set(CPPTRACE_DISABLE_ZSTD_DOWNLOAD     ON  CACHE BOOL "" FORCE)
set(CPPTRACE_GET_SYMBOLS_WITH_DBGHELP  ON  CACHE BOOL "" FORCE)
FetchContent_Declare(cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG        v0.7.5
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(spdlog cpptrace)

file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_RAW)
string(STRIP "${PROJECT_VERSION_RAW}" PROJECT_VERSION_RAW)
string(REPLACE "." ";" VERSION_LIST "${PROJECT_VERSION_RAW}")
list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE PROJECT_BUILD_NUMBER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()
if(NOT GIT_HASH)
    set(GIT_HASH "unknown")
endif()
if(NOT PROJECT_BUILD_NUMBER)
    set(PROJECT_BUILD_NUMBER "0")
endif()

set(PROJECT_VERSION_FULL "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_BUILD_NUMBER}.${GIT_HASH}")
message(STATUS "Project Version: ${PROJECT_VERSION_FULL}")

configure_file(cmake/templates/Version.hpp.in ${CMAKE_BINARY_DIR}/include/ate_parser/version.hpp @ONLY)
configure_file(cmake/templates/Version.cpp.in ${CMAKE_BINARY_DIR}/src/version.cpp @ONLY)

add_custom_target(UpdateVersion ALL
    COMMAND ${CMAKE_COMMAND}
        -DSOURCE_DIR=${CMAKE_SOURCE_DIR}
        -DBINARY_DIR=${CMAKE_BINARY_DIR}
        -P ${CMAKE_SOURCE_DIR}/cmake/scripts/UpdateVersion.cmake
    COMMENT "Updating Build Number..."
)
set_source_files_properties(${CMAKE_BINARY_DIR}/src/version.cpp PROPERTIES GENERATED TRUE)

if(WIN32)
    configure_file(cmake/templates/resource.rc.in ${CMAKE_BINARY_DIR}/resource.rc @ONLY)
    set(RESOURCE_FILE ${CMAKE_BINARY_DIR}/resource.rc)
endif()

set(ATEPARSER_LIB_SOURCES
    src/enums/prefix.cpp
    src/enums/test_status.cpp
    src/enums/field_type.cpp
    src/utils/safe_conversion.cpp
    src/records/analog.cpp
    src/records/digital.cpp
    src/core/record.cpp
    src/core/record_tree.cpp
    src/core/parser.cpp
    ${CMAKE_BINARY_DIR}/src/version.cpp
    ${RESOURCE_FILE}
)

add_library(ATEParser SHARED ${ATEPARSER_LIB_SOURCES})
add_dependencies(ATEParser UpdateVersion)

target_include_directories(ATEParser PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(ATEParser PUBLIC spdlog::spdlog cpptrace::cpptrace)

set_target_properties(ATEParser PROPERTIES
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(WIN32)
    set_target_properties(ATEParser PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_executable(ATEParserApp src/main.cpp ${ATEPARSER_LIB_SOURCES})
add_dependencies(ATEParserApp UpdateVersion)
target_include_directories(ATEParserApp PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(ATEParserApp PRIVATE spdlog::spdlog cpptrace::cpptrace)
set_target_properties(ATEParserApp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(WIN32)
    install(
        TARGETS ATEParser ATEParserApp
        RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES  "api-ms-win-.*" "ext-ms-win-.*"
            POST_EXCLUDE_REGEXES "api-ms-win-.*" "ext-ms-win-.*"
                                 "kernel32.dll" "KernelBase.dll" "msvcrt.dll" "ntdll.dll"
                                 "user32.dll" "gdi32.dll" "advapi32.dll" "shell32.dll"
                                 "ole32.dll" "oleaut32.dll" "ws2_32.dll"
            DIRECTORIES "${MINGW_BIN_DIR}"
        RUNTIME DESTINATION "bin"
    )
else()
    install(
        TARGETS ATEParser ATEParserApp
        LIBRARY DESTINATION "lib"
        RUNTIME DESTINATION "bin"
    )
endif()

enable_testing()
add_subdirectory(tests)

# ---- Benchmark -----------------------------------------------------------
add_executable(bench_parse benchmarks/bench_parse.cpp ${ATEPARSER_LIB_SOURCES})
add_dependencies(bench_parse UpdateVersion)
target_include_directories(bench_parse PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_BINARY_DIR}/include
)
target_link_libraries(bench_parse PRIVATE spdlog::spdlog cpptrace::cpptrace)
set_target_properties(bench_parse PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
