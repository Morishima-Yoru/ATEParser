cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Compiler Configuration (Windows/MinGW) ---
if(WIN32)
    set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc.exe")
    set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++.exe")
    
    # Find MinGW bin directory for runtime dependencies
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-prog-name=ld
        OUTPUT_VARIABLE MINGW_LD_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    get_filename_component(MINGW_BIN_DIR "${MINGW_LD_PATH}" DIRECTORY)
    message(STATUS "MINGW_BIN_DIR: ${MINGW_BIN_DIR}")
    
    # Static link standard libraries to avoid external dependencies (libgcc, libstdc++)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif()

project(ATEParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Versioning ---
file(READ "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_RAW)
string(STRIP "${PROJECT_VERSION_RAW}" PROJECT_VERSION_RAW)
string(REPLACE "." ";" VERSION_LIST "${PROJECT_VERSION_RAW}")
list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

# Get Git Hash
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
else()
    set(GIT_HASH "unknown")
    set(PROJECT_BUILD_NUMBER "0")
endif()

if("${GIT_HASH}" STREQUAL "")
    set(GIT_HASH "unknown")
endif()
if("${PROJECT_BUILD_NUMBER}" STREQUAL "")
    set(PROJECT_BUILD_NUMBER "0")
endif()

set(PROJECT_VERSION_FULL "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}.${PROJECT_BUILD_NUMBER}.${GIT_HASH}")
message(STATUS "Project Version: ${PROJECT_VERSION_FULL}")

# Configure Version files
# configure_file(cmake/templates/Version.hpp.in ${CMAKE_BINARY_DIR}/include/i3070/core/Version.hpp)
# configure_file(cmake/templates/Version.cpp.in ${CMAKE_BINARY_DIR}/src/core/Version.cpp)

# --- Auto-Increment Build Number ---
# Add a custom target that runs the update script BEFORE compiling
add_custom_target(UpdateVersion ALL
    COMMAND ${CMAKE_COMMAND} 
        -DSOURCE_DIR=${CMAKE_SOURCE_DIR} 
        -DBINARY_DIR=${CMAKE_BINARY_DIR} 
        -P ${CMAKE_SOURCE_DIR}/cmake/scripts/UpdateVersion.cmake
    COMMENT "Updating Build Number..."
)

# Ensure source files depend on the generated version file
set_source_files_properties(${CMAKE_BINARY_DIR}/src/core/Version.cpp PROPERTIES GENERATED TRUE)

if(WIN32)
    configure_file(cmake/templates/resource.rc.in ${CMAKE_BINARY_DIR}/resource.rc)
    set(RESOURCE_FILE ${CMAKE_BINARY_DIR}/resource.rc)
endif()

# --- Sources ---
set(ATEPARSER_LIB_SOURCES
    src/containers/LogRecordContainer.cpp
    src/containers/LogRecordFactory.cpp
    src/core/FieldValue.cpp
    src/core/I3070LogParser.cpp
    src/core/LogRecord.cpp
    ${CMAKE_BINARY_DIR}/src/core/Version.cpp
    src/enums/FieldType.cpp
    src/enums/LogRecordPrefix.cpp
    src/enums/TestStatus.cpp
    src/records/AnalogRecords.cpp
    src/records/DigitalRecords.cpp
    src/records/ShortRecords.cpp
    src/records/SystemRecords.cpp
    src/utils/ConfigReader.cpp
    src/utils/SafeConversion.cpp
    ${RESOURCE_FILE}
)

# --- Targets ---

# Shared Library (DLL/SO)
add_library(ATEParser SHARED ${ATEPARSER_LIB_SOURCES})

# Make ATEParser depend on UpdateVersion so it runs first
add_dependencies(ATEParser UpdateVersion)

# Include Directories
target_include_directories(ATEParser PUBLIC 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/third_party # For <nlohmann/json.hpp>
    ${CMAKE_BINARY_DIR}/include
)

# Target Properties
set_target_properties(ATEParser PROPERTIES 
    PREFIX "" # No 'lib' prefix on Windows
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if (WIN32)
    set_target_properties(ATEParser PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Executable
# Note: Compiling sources directly into executable to ensure it works standalone 
# and avoids MinGW export/import complexity for this tool.
add_executable(ATEParserApp src/main.cpp ${ATEPARSER_LIB_SOURCES})

# Include directories for Executable
target_include_directories(ATEParserApp PUBLIC 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/third_party 
    ${CMAKE_BINARY_DIR}/include
)

# Output executable to the same directory as the DLL
set_target_properties(ATEParserApp PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- Installation ---
if(WIN32)
    install(
        TARGETS ATEParser ATEParserApp
        RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-win-.*" "ext-ms-win-.*"
            POST_EXCLUDE_REGEXES "api-ms-win-.*" "ext-ms-win-.*" "kernel32.dll" "KernelBase.dll" "msvcrt.dll" "ntdll.dll" "user32.dll" "gdi32.dll" "advapi32.dll" "shell32.dll" "ole32.dll" "oleaut32.dll" "ws2_32.dll"
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
