cmake_minimum_required(VERSION 3.10)

# 設定 compiler
set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc.exe")
set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++.exe")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")

project(ATEParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
)

# Find source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ATEParserApp main.cpp ${SOURCES})

add_library(ATEParser SHARED
    src/containers/LogRecordContainer.cpp
    src/containers/LogRecordFactory.cpp
    src/core/FieldValue.cpp
    src/core/I3070LogParser.cpp
    src/core/LogRecord.cpp
    src/emums/FieldType.cpp
    src/emums/LogRecordPrefix.cpp
    src/emums/TestStatus.cpp
    src/records/AnalogRecords.cpp
    src/records/DigitalRecords.cpp
    src/records/ShortRecords.cpp
    src/records/SystemRecords.cpp
    src/utils/ConfigReader.cpp
    src/utils/SafeConversion.cpp
)

# Windows: export all symbols
if (WIN32)
    set_target_properties(ATEParser PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON PREFIX "")
endif()

# Include directories
target_include_directories(ATEParser PUBLIC include third_party/nlohmann)

# nlohmann json is header-only, so just include the path

# 產生 DLL 於 output_dll/，且無 lib 前綴
set_target_properties(ATEParser PROPERTIES
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output_dll"
)

get_filename_component(MINGW_BIN "${CMAKE_CXX_COMPILER}" DIRECTORY)
set(MINGW_DLLS
    libc++.dll
    libunwind.dll
    libwinpthread-1.dll
)
foreach(dll ${MINGW_DLLS})
    add_custom_command(TARGET ATEParser POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN}/${dll}"
            "$<TARGET_FILE_DIR:ATEParser>"
    )
endforeach()