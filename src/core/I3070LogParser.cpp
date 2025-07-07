#include "keysight_log/core/I3070LogParser.hpp"
#include "keysight_log/containers/LogRecordFactory.hpp"
#include "keysight_log/utils/JsonKeys.hpp"
#include "keysight_log/utils/ConfigReader.hpp"
#include <stack>
#include <stdexcept>
#include <iostream>
#include <mutex>
#include <algorithm>
#include <string_view>
#include <fstream>
#include <sstream>
#include <cstring>
#include "keysight_log/core/LogRecord.hpp"

using namespace std;
using namespace keysight_log::core;
using namespace keysight_log::containers;
using json = nlohmann::json;

namespace {
// Singleton ConfigReader, initialized only once
ConfigReader& getConfig() {
    static ConfigReader config("config.ini");
    return config;
}

bool isShowParser() {
    static bool cached = false;
    static once_flag flag;
    call_once(flag, []() {
        cached = getConfig().getBool("DEBUG", "SHOW_PARSER", false);
    });
    return cached;
}

// Optimized string trimming without creating temporary strings
inline void trimInPlace(string& s) {
    // Guard clause: empty string
    if (s.empty()) {
        return;
    }
    
    size_t start = s.find_first_not_of(" \t\r\n");
    // Guard clause: string contains only whitespace
    if (start == string::npos) {
        s.clear();
        return;
    }
    
    size_t end = s.find_last_not_of(" \t\r\n");
    s.erase(end + 1);
    s.erase(0, start);
}

// Fast prefix checking without substr
inline bool startsWith(const string& text, const string& prefix) {
    return text.size() >= prefix.size() && 
           text.compare(0, prefix.size(), prefix) == 0;
}

// Helper function to detect Analog Test records
inline bool isAnalogTestRecord(const string& recordText) {
    // Check for common Analog Test prefixes
    return startsWith(recordText, "@A-") || 
           startsWith(recordText, "@A-CAP") ||
           startsWith(recordText, "@A-DIO") ||
           startsWith(recordText, "@A-FUS") ||
           startsWith(recordText, "@A-IND") ||
           startsWith(recordText, "@A-JUM") ||
           startsWith(recordText, "@A-MEA") ||
           startsWith(recordText, "@A-NFE") ||
           startsWith(recordText, "@A-NPN") ||
           startsWith(recordText, "@A-PFE") ||
           startsWith(recordText, "@A-PNP") ||
           startsWith(recordText, "@A-POT") ||
           startsWith(recordText, "@A-RES") ||
           startsWith(recordText, "@A-SWI") ||
           startsWith(recordText, "@A-ZEN");
}

// Optimized field splitting with pre-allocated vector (string_view internal, string external)
inline vector<string> splitFields(const string& text, char delimiter, size_t startPos = 0) {
    std::string_view sv(text);
    if (sv.empty() || startPos >= sv.size()) {
        return {};
    }
    vector<string> fields;
    fields.reserve(16);
    size_t pos = startPos;
    size_t field_start = pos;
    int brace_depth = 0;
    while (pos < sv.size()) {
        char c = sv[pos];
        if (c == '{') {
            ++brace_depth;
        } else if (c == '}') {
            if (brace_depth > 0) --brace_depth;
        } else if (c == delimiter && brace_depth == 0) {
            fields.emplace_back(sv.substr(field_start, pos - field_start));
            field_start = pos + 1;
        }
        ++pos;
    }
    // Add the last field if there's remaining content
    if (field_start < sv.size()) {
        fields.emplace_back(sv.substr(field_start, sv.size() - field_start));
    }
    return fields;
}
}

I3070LogParser::I3070LogParser() {}
I3070LogParser::~I3070LogParser() {}

size_t I3070LogParser::findMatchingBrace(const string& text, size_t openPos) {
    // Guard clause: validate input
    if (openPos >= text.size() || text[openPos] != '{') {
        return string::npos;
    }
    
    int braceCount = 1;
    for (size_t i = openPos + 1; i < text.size(); ++i) {
        if (text[i] == '{') {
            ++braceCount;
        } else if (text[i] == '}') {
            --braceCount;
            if (braceCount == 0) {
                return i;
            }
        }
    }
    return string::npos;
}

pair<string, vector<string>> I3070LogParser::parseFields(const string& recordText) {
    // Guard clause: empty input
    if (recordText.empty()) {
        return {"", {}};
    }
    
    // Fast prefix checking for special record types
    if (startsWith(recordText, "@RPT")) {
        return parseRPTFields(recordText);
    }
    
    if (startsWith(recordText, "@PIN")) {
        return parsePINFields(recordText);
    }
    
    if (startsWith(recordText, "@TS-D")) {
        return parseTSDFields(recordText);
    }
    
    // Check for Analog Test records that may have optional subtest_designator
    if (isAnalogTestRecord(recordText)) {
        return parseAnalogTestFields(recordText);
    }
    
    // General record parsing with | delimiter
    size_t sep = recordText.find('|');
    if (sep == string::npos) {
        // No fields, just prefix
        return {recordText, {}};
    }
    
    string prefixStr = recordText.substr(0, sep);
    vector<string> fields;
    
    // Check for nested braces before processing fields
    size_t bracePos = recordText.find('{', sep + 1);
    if (bracePos != string::npos) {
        // Process fields up to the brace
        fields = splitFields(recordText, '|', sep + 1);
        // Restd::move the last field if it contains the brace
        if (!fields.empty() && (fields.back().find('{') != string::npos)) {
            fields.pop_back();
        }
    } else {
        // No nested content, process all fields
        fields = splitFields(recordText, '|', sep + 1);
    }
    
    return {prefixStr, fields};
}

pair<string, vector<string>> I3070LogParser::parseRPTFields(const string& recordText) {
    // Guard clause: validate @RPT format
    if (!startsWith(recordText, "@RPT")) {
        throw runtime_error("Invalid @RPT record format");
    }
    
    size_t tildePos = recordText.find('~');
    if (tildePos == string::npos) {
        return parseRPTFieldsSimple(recordText);
    }
    
    size_t pipePos = recordText.find('|', tildePos);
    if (pipePos == string::npos) {
        throw runtime_error("Invalid @RPT format: missing | after literal length");
    }
    
    // Parse literal length
    string literalLengthStr(recordText, tildePos + 1, pipePos - tildePos - 1);
    int literalLength;
    try {
        literalLength = stoi(literalLengthStr);
    } catch (const exception& e) {
        throw runtime_error("Invalid @RPT literal length: " + literalLengthStr);
    }
    
    // Extract literal field
    size_t literalStart = pipePos + 1;
    if (literalStart + literalLength > recordText.size()) {
        throw runtime_error("Invalid @RPT format: literal field length exceeds record size");
    }
    
    vector<string> fields;
    fields.reserve(8); // Pre-allocate common field count
    fields.emplace_back(recordText, literalStart, literalLength);
    
    // Parse remaining fields
    size_t remainingStart = literalStart + literalLength;
    if (remainingStart < recordText.size()) {
        auto remainingFields = parseRemainingRPTFields(recordText.substr(remainingStart));
        fields.insert(fields.end(), remainingFields.begin(), remainingFields.end());
    }
    
    return {"@RPT", fields};
}

pair<string, vector<string>> I3070LogParser::parseRPTFieldsSimple(const string& recordText) {
    // Guard clause: validate input
    if (!startsWith(recordText, "@RPT")) {
        throw runtime_error("Invalid @RPT record format");
    }
    
    auto fields = splitFields(recordText, '|', 5); // Skip "@RPT"
    return {"@RPT", fields};
}

vector<string> I3070LogParser::parseRemainingRPTFields(const string& remaining) {
    // Guard clause: empty input
    if (remaining.empty()) {
        return {};
    }
    
    return splitFields(remaining, '|');
}

pair<string, vector<string>> I3070LogParser::parsePINFields(const string& recordText) {
    // Guard clause: validate @PIN format
    if (!startsWith(recordText, "@PIN")) {
        throw runtime_error("Invalid @PIN record format");
    }
    
    size_t backslashPos = recordText.find('\\');
    if (backslashPos == string::npos) {
        return parsePINFieldsSimple(recordText);
    }
    
    size_t pipePos = recordText.find('|', backslashPos);
    if (pipePos == string::npos) {
        throw runtime_error("Invalid @PIN format: missing | after count");
    }
    
    vector<string> fields;
    fields.reserve(8); // Pre-allocate common field count
    
    // Add count field
    fields.emplace_back(recordText, backslashPos, pipePos - backslashPos);
    
    // Parse remaining pin fields
    size_t remainingStart = pipePos + 1;
    if (remainingStart < recordText.size()) {
        auto remainingFields = parseRemainingPINFields(recordText.substr(remainingStart));
        fields.insert(fields.end(), remainingFields.begin(), remainingFields.end());
    }
    
    return {"@PIN", fields};
}

pair<string, vector<string>> I3070LogParser::parsePINFieldsSimple(const string& recordText) {
    // Guard clause: validate input
    if (!startsWith(recordText, "@PIN")) {
        throw runtime_error("Invalid @PIN record format");
    }
    
    auto fields = splitFields(recordText, '|', 5); // Skip "@PIN"
    return {"@PIN", fields};
}

vector<string> I3070LogParser::parseRemainingPINFields(const string& remaining) {
    // Guard clause: empty input
    if (remaining.empty()) {
        return {};
    }
    
    return splitFields(remaining, '|');
}

// Special parsing for Analog Test records with optional subtest_designator
pair<string, vector<string>> I3070LogParser::parseAnalogTestFields(const string& recordText) {
    if (recordText.empty()) return {"", {}};
    size_t sep = recordText.find('|');
    if (sep == string::npos) return {recordText, {}};
    string prefixStr = recordText.substr(0, sep);
    vector<string> fields;
    fields.reserve(3);

    size_t start = sep + 1;
    // 1. test_status
    size_t next = recordText.find('|', start);
    if (next == string::npos) {
        fields.push_back(recordText.substr(start));
        fields.push_back(""); // measured_value missing
        fields.push_back(""); // subtest_designator missing
        return {prefixStr, fields};
    }
    fields.push_back(recordText.substr(start, next - start));
    start = next + 1;

    // 2. measured_value
    // Check if measured_value is followed by | or {
    size_t bracePos = recordText.find('{', start);
    size_t pipePos = recordText.find('|', start);
    if (pipePos == string::npos || (bracePos != string::npos && bracePos < pipePos)) {
        // No subtest_designator
        fields.push_back(recordText.substr(start, bracePos == string::npos ? string::npos : bracePos - start));
        fields.push_back(""); // subtest_designator missing
        return {prefixStr, fields};
    }
    // Has subtest_designator
    fields.push_back(recordText.substr(start, pipePos - start));
    start = pipePos + 1;
    // 3. subtest_designator
    bracePos = recordText.find('{', start);
    if (bracePos == string::npos) {
        fields.push_back(recordText.substr(start));
    } else {
        fields.push_back(recordText.substr(start, bracePos - start));
    }
    return {prefixStr, fields};
}

pair<string, vector<string>> I3070LogParser::parseTSDFields(const string& recordText) {
    // Guard clause: validate @TS-D format
    if (!startsWith(recordText, "@TS-D")) {
        throw runtime_error("Invalid @TS-D record format");
    }
    size_t backslashPos = recordText.find('\\');
    if (backslashPos == string::npos) {
        // 沒有 count，直接用一般分割
        size_t sep = recordText.find('|');
        if (sep == string::npos) {
            return {"@TS-D", {}};
        }
        auto fields = splitFields(recordText, '|', sep + 1);
        return {"@TS-D", fields};
    }
    size_t pipePos = recordText.find('|', backslashPos);
    if (pipePos == string::npos) {
        throw runtime_error("Invalid @TS-D format: missing | after count");
    }
    vector<string> fields;
    fields.reserve(8);
    // 第一欄: node_count (含在 prefix 內)
    fields.emplace_back(recordText, backslashPos, pipePos - backslashPos);
    // 其餘欄位
    size_t remainingStart = pipePos + 1;
    if (remainingStart < recordText.size()) {
        auto remainingFields = splitFields(recordText, '|', remainingStart);
        fields.insert(fields.end(), remainingFields.begin(), remainingFields.end());
    }
    return {"@TS-D", fields};
}

unique_ptr<LogRecordContainer> I3070LogParser::parse(const string& logText) {
    // Guard clause: empty input
    if (logText.empty()) {
        return make_unique<LogRecordContainer>(nullptr);
    }
    
    auto root = make_unique<LogRecordContainer>(nullptr);
    parseContainer(logText, *root);
    return root;
}

void I3070LogParser::parseContainer(const string& text, LogRecordContainer& container) {
    using std::string_view;
    string_view sv(text);
    size_t pos = 0;
    while (pos < sv.size()) {
        size_t openPos = sv.find('{', pos);
        if (openPos == string_view::npos) break;
        size_t closePos = findMatchingBrace(string(sv), openPos); // findMatchingBrace 仍用 string
        if (closePos == string_view::npos) throw runtime_error("Unbalanced braces");
        string_view recordText = sv.substr(openPos + 1, closePos - openPos - 1);
        size_t firstNested = recordText.find('{');
        size_t fieldEnd = firstNested;
        string_view flatFields = recordText.substr(0, fieldEnd);
        if (isShowParser()) {
            cerr << "[DEBUG] Record found. Raw text: '" << string(flatFields) << "'" << endl;
        }
        auto [prefixStr, fields] = parseFields(string(flatFields)); // parseFields 仍回傳 string
        trimInPlace(prefixStr);
        if (prefixStr.empty()) {
            pos = closePos + 1;
            continue;
        }
        if (isShowParser()) {
            cerr << "[DEBUG] Record found. Prefix: " << prefixStr << ", Raw text: '" << flatFields << "'" << endl;
        }
        auto prefix = LogRecordFactory::stringToPrefix(prefixStr);
        auto record = LogRecordFactory::createRecord(prefix);
        record->raw_prefix = prefixStr;
        record->raw_data = string(flatFields);
        record->fromFields(fields);
        auto node = make_unique<LogRecordContainer>(std::move(record));
        if (fieldEnd < recordText.size()) {
            parseContainer(string(recordText.substr(fieldEnd)), *node);
        }
        container.addSubrecord(std::move(node));
        pos = closePos + 1;
    }
}

json I3070LogParser::containerToJson(const LogRecordContainer& container) {
    auto rec = container.getRecord();
    
    // Guard clause: root container without record
    if (!rec) {
        auto& subrecords = container.getSubrecords();
        if (!subrecords.empty()) {
            return containerToJson(*subrecords[0]);
        }
        return json::object();
    }
    
    // Process record with children
    json j = rec->toJson();
    
    auto& subrecords = container.getSubrecords();
    // Guard clause: no subrecords
    if (subrecords.empty()) {
        return j;
    }
    
    json children = json::array();
    
    // Pre-allocate vector for better performance, then convert to json
    vector<json> childVector;
    childVector.reserve(subrecords.size());
    
    for (auto& sub : subrecords) {
        childVector.push_back(containerToJson(*sub));
    }
    
    // Convert vector to json array
    children = json(childVector);
    j[JSON_KEY_SUBRECORDS] = std::move(children);
    
    return j;
}

extern "C" DLL_EXPORT const char* parse_file(const char* log_filepath, const char* dst_filepath, int indent, bool keep_raw) {
    using namespace keysight_log::core;
    // 設定是否保留 raw field
    LogRecord::show_raw_field = keep_raw;
    // 讀取 log 檔案
    std::ifstream ifs(log_filepath);
    if (!ifs) {
        static std::string err = "Cannot open log file";
        return err.c_str();
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string logText = buffer.str();
    // 解析 log
    I3070LogParser parser;
    auto tree = parser.parse(logText);
    auto json = I3070LogParser::containerToJson(*tree);
    std::string json_str = json.dump(indent);
    // 若有指定輸出檔案
    if (dst_filepath && std::strlen(dst_filepath) > 0) {
        std::ofstream ofs(dst_filepath);
        ofs << json_str;
        ofs.close();
    }
    // 回傳字串（需由 Python 負責釋放）
    char* result = new char[json_str.size() + 1];
    std::memcpy(result, json_str.c_str(), json_str.size() + 1);
    return result;
}

extern "C" DLL_EXPORT void free_mem(const char* ptr) {
    delete[] ptr;
}

extern "C" DLL_EXPORT const char* parse_log_string(const char* log_content, const char* dst_filepath, int indent, bool keep_raw) {
    using namespace keysight_log::core;
    LogRecord::show_raw_field = keep_raw;
    std::string logText = log_content ? log_content : "";
    I3070LogParser parser;
    auto tree = parser.parse(logText);
    auto json = I3070LogParser::containerToJson(*tree);
    std::string json_str = json.dump(indent);
    if (dst_filepath && std::strlen(dst_filepath) > 0) {
        std::ofstream ofs(dst_filepath);
        ofs << json_str;
        ofs.close();
    }
    char* result = new char[json_str.size() + 1];
    std::memcpy(result, json_str.c_str(), json_str.size() + 1);
    return result;
} 
