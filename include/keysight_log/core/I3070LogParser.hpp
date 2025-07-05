#ifndef KEYSIGHT_LOG_CORE_I3070LOGPARSER_HPP
#define KEYSIGHT_LOG_CORE_I3070LOGPARSER_HPP

#include <string>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>
#include "keysight_log/containers/LogRecordContainer.hpp"
using namespace std;
namespace keysight_log {
namespace core {

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT
#endif

extern "C" DLL_EXPORT const char* parse_file(const char* log_filepath, const char* dst_filepath, int indent, bool keep_raw);
extern "C" DLL_EXPORT void free_mem(const char* ptr);
extern "C" DLL_EXPORT const char* parse_log_string(const char* log_content, const char* dst_filepath, int indent, bool keep_raw);

class I3070LogParser {
public:
    using json = nlohmann::json;
    I3070LogParser();
    ~I3070LogParser();

    // Parse entire log and return container tree
    unique_ptr<containers::LogRecordContainer> parse(const string& logText);

    // Parse single record and return prefix and fields
    static pair<string, vector<string>> parseFields(const string& recordText);

    // Convert container tree to JSON
    static json containerToJson(const containers::LogRecordContainer& container);

private:
    // Helper: find matching closing brace
    static size_t findMatchingBrace(const string& text, size_t openPos);
    // Recursively parse nested containers
    void parseContainer(const string& text, containers::LogRecordContainer& container);
    
    // @RPT record special parsing methods
    static pair<string, vector<string>> parseRPTFields(const string& recordText);
    static pair<string, vector<string>> parseRPTFieldsSimple(const string& recordText);
    static vector<string> parseRemainingRPTFields(const string& remaining);
    
    // @PIN record special parsing methods
    static pair<string, vector<string>> parsePINFields(const string& recordText);
    static pair<string, vector<string>> parsePINFieldsSimple(const string& recordText);
    static vector<string> parseRemainingPINFields(const string& remaining);
    
    // Analog Test record special parsing methods
    static pair<string, vector<string>> parseAnalogTestFields(const string& recordText);
};

} // namespace core
} // namespace keysight_log

#endif // KEYSIGHT_LOG_CORE_I3070LOGPARSER_HPP 