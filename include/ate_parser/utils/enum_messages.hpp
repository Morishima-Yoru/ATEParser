/**
 * @file enum_messages.hpp
 * @brief Constexpr string constants for enum display labels and diagnostics.
 *
 * Single source of truth for:
 *   - to_string() return values for AnalogTestStatus, DigitalTestStatus,
 *     GenericTestStatus, and FieldType.
 *   - Exception message prefixes thrown by to_*_status() conversion helpers.
 *   - Input token strings accepted by to_field_type() (primary + aliases).
 */
#pragma once

#include <string_view>

namespace ate::enum_msg {

// ---- AnalogTestStatus display labels -------------------------------------

inline constexpr std::string_view k_analog_passed            = "Passed";
inline constexpr std::string_view k_analog_failed            = "Failed";
inline constexpr std::string_view k_analog_failed_compliance = "Failed (Compliance Limit)";
inline constexpr std::string_view k_analog_failed_detector   = "Failed (Detector Timeout)";
inline constexpr std::string_view k_analog_failed_general    = "Failed (General)";
inline constexpr std::string_view k_analog_aborted_operator  = "Aborted by Operator";
inline constexpr std::string_view k_analog_unknown           = "Unknown Analog Status";

// ---- DigitalTestStatus display labels ------------------------------------

inline constexpr std::string_view k_digital_passed           = "Passed";
inline constexpr std::string_view k_digital_failed           = "Failed";
inline constexpr std::string_view k_digital_crc_failure      = "CRC Failure";
inline constexpr std::string_view k_digital_fatal_error      = "Fatal Error";
inline constexpr std::string_view k_digital_chain_integrity  = "Chain Integrity Failure";
inline constexpr std::string_view k_digital_unknown          = "Unknown Digital Status";

// ---- GenericTestStatus display labels ------------------------------------

inline constexpr std::string_view k_generic_pass             = "Pass";
inline constexpr std::string_view k_generic_fail             = "Fail";
inline constexpr std::string_view k_generic_fatal_error      = "Fatal Error";
inline constexpr std::string_view k_generic_learning_passed  = "Learning Passed";
inline constexpr std::string_view k_generic_unknown          = "Unknown Generic Status";

// ---- FieldType display labels --------------------------------------------

inline constexpr std::string_view k_field_bool    = "bool";
inline constexpr std::string_view k_field_fp      = "fp";
inline constexpr std::string_view k_field_int     = "int";
inline constexpr std::string_view k_field_str     = "str";
inline constexpr std::string_view k_field_list    = "list";
inline constexpr std::string_view k_field_literal = "literal";
inline constexpr std::string_view k_field_unknown = "unknown";

// ---- to_field_type() alternative input tokens ----------------------------
// Primary tokens share the display label constants above.

inline constexpr std::string_view k_token_boolean  = "boolean";
inline constexpr std::string_view k_token_float    = "float";
inline constexpr std::string_view k_token_double   = "double";
inline constexpr std::string_view k_token_integer  = "integer";
inline constexpr std::string_view k_token_string   = "string";
inline constexpr std::string_view k_token_array    = "array";
inline constexpr std::string_view k_token_raw      = "raw";

// ---- Conversion exception prefixes ---------------------------------------

/// Prefix for std::out_of_range thrown by to_analog_status().
inline constexpr std::string_view k_invalid_analog_code  = "Invalid AnalogTestStatus code: ";

/// Prefix for std::out_of_range thrown by to_digital_status().
inline constexpr std::string_view k_invalid_digital_code = "Invalid DigitalTestStatus code: ";

/// Prefix for std::out_of_range thrown by to_generic_status().
inline constexpr std::string_view k_invalid_generic_code = "Invalid GenericTestStatus code: ";

} // namespace ate::enum_msg
