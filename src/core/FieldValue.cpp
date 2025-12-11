/**
 * @file FieldValue.cpp
 * @brief Implements parsing, serialization, and type utilities for FieldValue
 *
 * This source defines:
 *  - parseFieldValue(): Convert a token to FieldValue using type hints
 *  - fieldValueToString(): Serialize FieldValue back to string form
 *  - getFieldValueType(): Determine type hint character for a FieldValue
 *
 * Based strictly on Keysight i3070 Log Record Format (Tables 8-2, 8-3) [1].
 * @date 2025-06-19
 */

#include "i3070/core/FieldValue.hpp"
#include <nlohmann/json.hpp>
#include <charconv>
#include <cctype>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace core {

static bool parseBool(const string& tok, bool& out) {
    if (tok == "1" || tok == "Y" || tok == "y") {
        out = true;
        return true;
    }
    if (tok == "0" || tok == "N" || tok == "n") {
        out = false;
        return true;
    }
    return false;
}

optional<FieldValue> parseFieldValue(const string& token, char typeHint) {
    switch (typeHint) {
        case 'b': {
            bool val;
            if (parseBool(token, val)) {
                return val;
            }
            return nullopt;
        }
        case 'i': {
            int v = 0;
            auto result = from_chars(token.data(), token.data() + token.size(), v);
            if (result.ec == errc()) {
                return v;
            }
            return nullopt;
        }
        case 'f': {
            try {
                double d = stod(token);
                return d;
            } catch (...) {
                return nullopt;
            }
        }
        case 's': {
            // String fields accept raw content, including empty tokens
            return token;
        }
        default:
            return nullopt;
    }
}

string fieldValueToString(const FieldValue& value) {
    if (auto p = get_if<bool>(&value)) {
        return *p ? "1" : "0";
    }
    if (auto p = get_if<int>(&value)) {
        return to_string(*p);
    }
    if (auto p = get_if<double>(&value)) {
        // Use scientific notation to match log format
        char buf[32];
        snprintf(buf, sizeof(buf), "%.6E", *p);
        return string(buf);
    }
    if (auto p = get_if<string>(&value)) {
        return *p;
    }
    return {};
}

char getFieldValueType(const FieldValue& value) {
    if (holds_alternative<bool>(value)) {
        return 'b';
    }
    if (holds_alternative<int>(value)) {
        return 'i';
    }
    if (holds_alternative<double>(value)) {
        return 'f';
    }
    return 's';
}

} // namespace core
} // namespace i3070
