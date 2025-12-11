/**
 * @file FieldType.cpp
 * @brief Implements field type utilities and conversion functions
 *
 * This source file defines:
 *  - String↔enum conversions for FieldType
 *  - Field value parsing and validation functions
 *  - Type checking and description utilities
 *
 * Based strictly on Keysight i3070 Log Record Format (Tables 8-2, 8-3)[1].
 */

#include "i3070/enums/FieldType.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cctype>
#include <charconv>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace enums {

// Convert string to FieldType enum
FieldType stringToFieldType(const std::string& type_str) {
    std::string lower_str = type_str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
    
    if (lower_str == "bool" || lower_str == "boolean") {
        return FieldType::BOOL;
    }
    if (lower_str == "fp" || lower_str == "float" || lower_str == "double") {
        return FieldType::FP;
    }
    if (lower_str == "int" || lower_str == "integer") {
        return FieldType::INT;
    }
    if (lower_str == "str" || lower_str == "string") {
        return FieldType::STR;
    }
    if (lower_str == "list" || lower_str == "array") {
        return FieldType::LIST;
    }
    if (lower_str == "literal" || lower_str == "raw") {
        return FieldType::LITERAL;
    }
    return FieldType::UNKNOWN;
}

// Convert FieldType enum to string
std::string fieldTypeToString(FieldType field_type) {
    switch (field_type) {
        case FieldType::BOOL:    return "bool";
        case FieldType::FP:      return "fp";
        case FieldType::INT:     return "int";
        case FieldType::STR:     return "str";
        case FieldType::LIST:    return "list";
        case FieldType::LITERAL: return "literal";
        default:                 return "unknown";
    }
}

// Parse string value into FieldValue based on target type
std::optional<FieldValue> parseStringToFieldValue(const std::string& value_str, FieldType target_type) {
    switch (target_type) {
        case FieldType::BOOL: {
            std::string lower_val = value_str;
            std::transform(lower_val.begin(), lower_val.end(), lower_val.begin(), ::tolower);
            if (lower_val == "1" || lower_val == "y" || lower_val == "true") {
                return true;
            }
            if (lower_val == "0" || lower_val == "n" || lower_val == "false") {
                return false;
            }
            return std::nullopt;
        }
        case FieldType::INT: {
            int value = 0;
            auto result = std::from_chars(value_str.data(), value_str.data() + value_str.size(), value);
            if (result.ec == std::errc{} && result.ptr == value_str.data() + value_str.size()) {
                return value;
            }
            return std::nullopt;
        }
        case FieldType::FP: {
            try {
                double value = std::stod(value_str);
                return value;
            } catch (const std::exception&) {
                return std::nullopt;
            }
        }
        case FieldType::STR: {
            return value_str;
        }
        default:
            return nullopt;
    }
}

// Convert FieldValue to string representation
std::string fieldValueToString(const FieldValue& value) {
    if (auto bool_val = get_if<bool>(&value)) {
        return *bool_val ? "1" : "0";
    }
    if (auto int_val = get_if<int>(&value)) {
        return to_string(*int_val);
    }
    if (auto double_val = get_if<double>(&value)) {
        // Use scientific notation with 6 significant digits as per i3070 format
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.6E", *double_val);
        return std::string(buffer);
    }
    if (auto str_val = get_if<std::string>(&value)) {
        return *str_val;
    }
    return "";
}

// Get FieldType of a FieldValue variant
FieldType getFieldValueType(const FieldValue& value) {
    if (holds_alternative<bool>(value)) {
        return FieldType::BOOL;
    }
    if (holds_alternative<int>(value)) {
        return FieldType::INT;
    }
    if (holds_alternative<double>(value)) {
        return FieldType::FP;
    }
    if (holds_alternative<std::string>(value)) {
        return FieldType::STR;
    }
    return FieldType::UNKNOWN;
}

// Check if field type represents numeric value
bool isNumericType(FieldType field_type) {
    return field_type == FieldType::INT || field_type == FieldType::FP;
}

// Check if field type represents composite structure
bool isCompositeType(FieldType field_type) {
    return field_type == FieldType::LIST || field_type == FieldType::LITERAL;
}

// Validate if string can be parsed as specified field type
bool validateFieldValue(const std::string& value_str, FieldType target_type) {
    return parseStringToFieldValue(value_str, target_type).has_value();
}

// Get human-readable description of field type
std::string getFieldTypeDescription(FieldType field_type) {
    switch (field_type) {
        case FieldType::BOOL:
            return "Boolean value (1/0, Y/N, true/false)";
        case FieldType::FP:
            return "Floating-point number (IEEE 754 double precision)";
        case FieldType::INT:
            return "Integer value (signed 32-bit integer)";
        case FieldType::STR:
            return "String value (UTF-8 encoded text)";
        case FieldType::LIST:
            return "List of values (array of other field types)";
        case FieldType::LITERAL:
            return "Literal data block with specified length";
        default:
            return "Unknown or unrecognized field type";
    }
}

// Get size in bytes for fixed-size field types
int getFieldTypeSize(FieldType field_type) {
    switch (field_type) {
        case FieldType::BOOL: return 1;
        case FieldType::INT:  return 4;
        case FieldType::FP:   return 8;
        case FieldType::STR:
        case FieldType::LIST:
        case FieldType::LITERAL:
        default:
            return -1; // Variable size
    }
}

} // namespace enums
} // namespace i3070
