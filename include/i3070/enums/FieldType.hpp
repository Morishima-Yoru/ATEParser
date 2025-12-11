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

#pragma once

#include <string>
#include <variant>
#include <vector>

namespace i3070::enums {

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
using FieldValue = std::variant<bool, int, double, std::string>;

/**
 * @struct ListField
 * @brief Structure for handling list-type fields
 * 
 * Represents a field that contains multiple values of potentially
 * different types. The count indicates the number of items in the list.
 */
struct ListField {
    int count;                          ///< Number of items in the list
    std::vector<FieldValue> items;      ///< Vector of field values
    
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
    int length;         ///< Specified byte count
    std::string data;   ///< Raw content, exactly length bytes
    
    LiteralField() : length(0) {}
    explicit LiteralField(const std::string& s)
      : length(static_cast<int>(s.size())), data(s) {}
};

} // namespace i3070::enums
