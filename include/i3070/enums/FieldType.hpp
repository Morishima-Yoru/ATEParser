/**
 * @file FieldType.hpp
 * @brief Defines field data types used in Keysight i3070 ICT log records
 * 
 * This header contains the enumeration and utilities for handling different
 * data field types that can appear in Keysight i3070 ICT log records.
 * The system supports four fundamental data types for field values.
 * 
 * @author Keysight Log Parser Team
 * @version 1.0
 * @date 2025-06-19
 */

#ifndef I3070_ENUMS_FIELDTYPE_HPP
#define I3070_ENUMS_FIELDTYPE_HPP

#include <string>
#include <variant>
#include <vector>
#include <optional>

using namespace std;

namespace i3070 {
namespace enums {

/**
 * @enum FieldType
 * @brief Enumeration of supported data field types in i3070 log records
 * 
 * This enumeration defines the four fundamental data types that can be
 * stored in log record fields. Each type corresponds to a specific
 * data representation format in the log files.
 */
enum class FieldType {
    // ========================================================================
    // BASIC DATA TYPES
    // These are the fundamental data types supported by the i3070 system
    // ========================================================================
    
    BOOL,       ///< Boolean value (represented as 1/0, Y/N, true/false)
    FP,         ///< Floating-point number (IEEE 754 double precision)
    INT,        ///< Integer value (signed 32-bit integer)
    STR,        ///< String value (UTF-8 encoded text)
    
    // ========================================================================
    // COMPOSITE DATA TYPES
    // These types represent more complex data structures
    // ========================================================================
    
    LIST,       ///< List of values (array of other field types)
    LITERAL,    ///< Literal data block with specified length
    
    // ========================================================================
    // SPECIAL TYPES
    // These are used for special cases or unknown data
    // ========================================================================
    
    UNKNOWN     ///< Unknown or unrecognized field type
};

/**
 * @brief Type alias for field value variant
 * 
 * This variant can hold any of the supported basic data types.
 * Used as the fundamental storage type for field values.
 */
using FieldValue = variant<bool, int, double, string>;

/**
 * @struct ListField
 * @brief Structure for handling list-type fields
 * 
 * Represents a field that contains multiple values of potentially
 * different types. The count indicates the number of items in the list.
 */
struct ListField {
    int count;                          ///< Number of items in the list
    vector<FieldValue> items;      ///< Vector of field values
    
    /**
     * @brief Default constructor
     */
    ListField() : count(0) {}
    
    /**
     * @brief Constructor with initial capacity
     * @param initial_count Expected number of items
     */
    explicit ListField(int initial_count) : count(initial_count) {
        items.reserve(initial_count);
    }
};

/**
 * @struct LiteralField
 * @brief Structure for handling literal data blocks
 * 
 * Represents a field that contains raw binary or text data
 * with a specified length. Used for custom or proprietary data formats.
 */
struct LiteralField {
    int length;                         ///< Length of the data in bytes
    string data;                   ///< Raw data content
    
    /**
     * @brief Default constructor
     */
    LiteralField() : length(0) {}
    
    /**
     * @brief Constructor with data
     * @param data_content The raw data content
     */
    explicit LiteralField(const string& data_content) 
        : length(static_cast<int>(data_content.size())), data(data_content) {}
};

/**
 * @brief Converts a string representation to FieldType enum value
 * @param type_str The string representation of the field type
 * @return The corresponding FieldType enum value
 * @note Returns FieldType::UNKNOWN for unrecognized types
 * 
 * Recognized strings include:
 * - "BOOL", "bool", "boolean" -> FieldType::BOOL
 * - "FP", "fp", "float", "double" -> FieldType::FP
 * - "INT", "int", "integer" -> FieldType::INT
 * - "STR", "str", "string" -> FieldType::STR
 * - "LIST", "list", "array" -> FieldType::LIST
 * - "LITERAL", "literal", "raw" -> FieldType::LITERAL
 */
FieldType stringToFieldType(const string& type_str);

/**
 * @brief Converts a FieldType enum value to its string representation
 * @param field_type The FieldType enum value
 * @return The string representation of the field type
 */
string fieldTypeToString(FieldType field_type);

/**
 * @brief Attempts to parse a string value into the specified field type
 * @param value_str The string representation of the value
 * @param target_type The desired field type to parse into
 * @return An optional FieldValue containing the parsed value, or nullopt if parsing failed
 * 
 * This function handles type conversion and validation:
 * - BOOL: Accepts "1", "0", "Y", "N", "true", "false" (case-insensitive)
 * - FP: Parses floating-point numbers using standard notation
 * - INT: Parses integer values within 32-bit signed range
 * - STR: Always succeeds, returns the input string as-is
 */
optional<FieldValue> parseStringToFieldValue(const string& value_str, FieldType target_type);

/**
 * @brief Converts a FieldValue to its string representation
 * @param value The FieldValue to convert
 * @return String representation of the value
 * 
 * Conversion rules:
 * - bool: Returns "1" or "0"
 * - int: Returns decimal string representation
 * - double: Returns string with appropriate precision
 * - string: Returns the string as-is
 */
string fieldValueToString(const FieldValue& value);

/**
 * @brief Gets the FieldType of a FieldValue variant
 * @param value The FieldValue to inspect
 * @return The FieldType corresponding to the held value type
 */
FieldType getFieldValueType(const FieldValue& value);

/**
 * @brief Checks if a field type represents a numeric value
 * @param field_type The FieldType to check
 * @return true if the field type is INT or FP, false otherwise
 */
bool isNumericType(FieldType field_type);

/**
 * @brief Checks if a field type represents a composite structure
 * @param field_type The FieldType to check
 * @return true if the field type is LIST or LITERAL, false otherwise
 */
bool isCompositeType(FieldType field_type);

/**
 * @brief Validates if a string can be parsed as the specified field type
 * @param value_str The string to validate
 * @param target_type The target field type
 * @return true if the string can be successfully parsed, false otherwise
 */
bool validateFieldValue(const string& value_str, FieldType target_type);

/**
 * @brief Gets a human-readable description of the field type
 * @param field_type The FieldType to describe
 * @return A descriptive string explaining the field type
 */
string getFieldTypeDescription(FieldType field_type);

/**
 * @brief Gets the size in bytes for fixed-size field types
 * @param field_type The FieldType to get size for
 * @return The size in bytes, or -1 for variable-size types
 * 
 * Returns:
 * - BOOL: 1 byte
 * - INT: 4 bytes
 * - FP: 8 bytes
 * - STR, LIST, LITERAL: -1 (variable size)
 */
int getFieldTypeSize(FieldType field_type);

/**
 * @brief Template function to safely extract typed value from FieldValue
 * @tparam T The target type to extract
 * @param value The FieldValue to extract from
 * @return Optional containing the extracted value, or nullopt if type mismatch
 * 
 * Example usage:
 * @code
 * FieldValue value = 42;
 * auto int_val = extractValue<int>(value);
 * if (int_val.has_value()) {
 *     cout << "Integer value: " << int_val.value() << endl;
 * }
 * @endcode
 */
template<typename T>
optional<T> extractValue(const FieldValue& value) {
    if (holds_alternative<T>(value)) {
        return get<T>(value);
    }
    return nullopt;
}

} // namespace enums
} // namespace i3070

#endif // I3070_ENUMS_FIELDTYPE_HPP
