cmake_minimum_required(VERSION 3.21)

# 設定 compiler
set(CMAKE_C_COMPILER "x86_64-w64-mingw32-gcc.exe")
set(CMAKE_CXX_COMPILER "x86_64-w64-mingw32-g++.exe")
if(WIN32)
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-prog-name=ld
        OUTPUT_VARIABLE MINGW_LD_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    get_filename_component(MINGW_BIN_DIR "${MINGW_LD_PATH}" DIRECTORY)
    message(STATUS "MINGW_BIN_DIR: ${MINGW_BIN_DIR}")
endif()

# message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
# message(STATUS "CMAKE_CXX_COMPILER_PREFIX: ${MINGW_BIN_DIR}")
# 如果需要動態連結 C++ 標準庫（會產生 DLL 依賴）
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
set_target_properties(
    ATEParser PROPERTIES
    PREFIX ""
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/output_dll"
)
# 安裝 DLL 及其依賴到 output_dll 目錄
install(
    TARGETS ATEParser 
    RUNTIME_DEPENDENCIES
        PRE_EXCLUDE_REGEXES "api-ms-win-.*"
        PRE_EXCLUDE_REGEXES "ext-ms-win-.*"
        POST_EXCLUDE_REGEXES "api-ms-win-.*"
        POST_EXCLUDE_REGEXES "ext-ms-win-.*"
        POST_EXCLUDE_REGEXES "kernel32.dll"
        POST_EXCLUDE_REGEXES "KernelBase.dll"
        POST_EXCLUDE_REGEXES "msvcrt.dll"
        POST_EXCLUDE_REGEXES "ntdll.dll"
        POST_EXCLUDE_REGEXES "user32.dll"
        POST_EXCLUDE_REGEXES "gdi32.dll"
        POST_EXCLUDE_REGEXES "advapi32.dll"
        POST_EXCLUDE_REGEXES "shell32.dll"
        POST_EXCLUDE_REGEXES "ole32.dll"
        POST_EXCLUDE_REGEXES "oleaut32.dll"
        POST_EXCLUDE_REGEXES "ws2_32.dll"
        DIRECTORIES "${MINGW_BIN_DIR}"
    RUNTIME DESTINATION "output_dll"
)


# # 手動複製常見的 MinGW DLL（如果 RUNTIME_DEPENDENCIES 不工作）
# if(WIN32)
#     # 找到 MinGW 的 bin 目錄
#     execute_process(
#         COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=libstdc++-6.dll
#         OUTPUT_VARIABLE MINGW_BIN_DIR
#         OUTPUT_STRIP_TRAILING_WHITESPACE
#     )
#     get_filename_component(MINGW_BIN_DIR ${MINGW_BIN_DIR} DIRECTORY)
    
#     # 複製常見的 DLL
#     add_custom_command(
#         TARGET ATEParser POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#             "${MINGW_BIN_DIR}/libstdc++-6.dll"
#             "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
#             "${MINGW_BIN_DIR}/libwinpthread-1.dll"
#             "${CMAKE_BINARY_DIR}/output_dll/"
#         COMMENT "Copying MinGW runtime DLLs"
#     )
# endif()
