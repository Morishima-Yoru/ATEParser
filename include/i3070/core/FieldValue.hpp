/**
 * @file FieldValue.hpp
 * @brief Defines the FieldValue variant and utilities for i3070 log fields
 * 
 * This header provides:
 *  - A type-safe variant for log field values (bool, int, double, string)
 *  - Structures for list fields and literal fields
 *  - Parsing, serialization, and inspection helper functions
 * 
 * Based strictly on Keysight i3070 Log Record Format (Tables 8-2, 8-3)  
 * @date 2025-06-19
 */

#pragma once

#include <variant>
#include <string>
#include <vector>
#include <optional>

namespace i3070::core {

/**
 * @brief Variant to hold any supported log field value
 * 
 * - bool   : boolean (1/0, Y/N)  
 * - int    : signed integer  
 * - double : floating-point number  
 * - string : text or raw literal content  
 */
using FieldValue = std::variant<bool, int, double, std::string>;

/**
 * @struct ListField
 * @brief Represents a backslash-delimited list field ("\N|item1|item2…")
 * 
 * The first element after '\' is the item count, followed by that many FieldValue items.
 */
struct ListField {
    int count;                          ///< Number of list items  
    std::vector<FieldValue> items;      ///< Parsed list entries  

    ListField(): count(0) {}
    explicit ListField(int n): count(n) { items.reserve(n); }
};

/**
 * @struct LiteralField
 * @brief Represents a tilde-delimited literal ("~len|raw…") field
 * 
 * Literal fields include special characters and have a fixed byte length.
 */
struct LiteralField {
    int length;         ///< Specified byte count  
    std::string data;   ///< Raw content, exactly length bytes  

    LiteralField(): length(0) {}
    explicit LiteralField(const std::string& s)
      : length(static_cast<int>(s.size())), data(s) {}
};


/**
 * @brief Serializes a FieldValue back into its string form
 * @param value  The FieldValue to serialize  
 * @return String representation fitting log format  
 */
std::string fieldValueToString(const FieldValue& value);

/**
 * @brief Determines the type hint character for a FieldValue
 * @param value  The FieldValue to inspect  
 * @return 'b', 'i', 'f', or 's' corresponding to the held type  
 */
char getFieldValueType(const FieldValue& value);

/**
 * @brief Attempts to extract a specific type from FieldValue safely
 * @tparam T   bool, int, double, or string  
 * @param v    The FieldValue variant  
 * @return Optional<T> containing the value or nullopt if type mismatch  
 */
template<typename T>
std::optional<T> getValue(const FieldValue& v) {
    if (auto p = std::get_if<T>(&v)) return *p;
    return std::nullopt;
}

} // namespace i3070::core
