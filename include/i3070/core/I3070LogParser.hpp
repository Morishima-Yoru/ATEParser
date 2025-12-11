#pragma once

#include <string>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>
#include "i3070/containers/LogRecordContainer.hpp"

namespace i3070::core {

enum ParseResult : int {
    Success = 0,
    InputError = 1,       // For file not found, empty log content, etc.
    ParseError = 2,       // For errors during the parsing process
    JsonConversionError = 3, // For errors converting to JSON
    OutputError = 4,       // For file write errors
    FailedIntegrity = 5,   // Added for integrity check failure
};

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

extern "C" DLL_EXPORT ParseResult parse_file(const char* log_filepath, const char* dst_filepath, int indent, bool keep_raw, char** out_json_str);
extern "C" DLL_EXPORT void free_mem(char* ptr);
extern "C" DLL_EXPORT ParseResult parse_log_string(const char* log_content, const char* dst_filepath, int indent, bool keep_raw, char** out_json_str);

class I3070LogParser {
public:
    using json = nlohmann::json;
    
    // Debug flags
    static bool show_parser_debug;

    I3070LogParser();
    ~I3070LogParser();

    // Parse entire log and return container tree
    std::unique_ptr<containers::LogRecordContainer> parse(const std::string& logText);

    // Parse single record and return prefix and fields
    static std::pair<std::string, std::vector<std::string>> parseFields(const std::string& recordText);

    // Convert container tree to JSON
    static json containerToJson(const containers::LogRecordContainer& container);

private:
    // Helper: find matching closing brace
    static size_t findMatchingBrace(const std::string& text, size_t openPos);
    // Recursively parse nested containers
    void parseContainer(const std::string& text, containers::LogRecordContainer& container);
    
    // @RPT record special parsing methods
    static std::pair<std::string, std::vector<std::string>> parseRPTFields(const std::string& recordText);
    static std::pair<std::string, std::vector<std::string>> parseRPTFieldsSimple(const std::string& recordText);
    static std::vector<std::string> parseRemainingRPTFields(const std::string& remaining);
    
    // @PIN record special parsing methods
    static std::pair<std::string, std::vector<std::string>> parsePINFields(const std::string& recordText);
    static std::pair<std::string, std::vector<std::string>> parsePINFieldsSimple(const std::string& recordText);
    static std::vector<std::string> parseRemainingPINFields(const std::string& remaining);
    
    // Analog Test record special parsing methods
    static std::pair<std::string, std::vector<std::string>> parseAnalogTestFields(const std::string& recordText);
    // @TS-D record special parsing methods
    static std::pair<std::string, std::vector<std::string>> parseTSDFields(const std::string& recordText);
};

} // namespace i3070::core
