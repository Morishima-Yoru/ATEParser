/**
 * @file conversion_messages.hpp
 * @brief Constexpr string constants for safe_conversion diagnostics.
 *
 * Centralises the boolean input token set, the boolean type label, and
 * the spdlog format templates used by parse_bool(), parse_or_default(),
 * parse_or_throw(), and parse_bool_or_default() in safe_conversion.cpp.
 */
#pragma once

#include <string_view>

namespace ate::conv_msg {

// ---- Boolean input tokens ------------------------------------------------

inline constexpr std::string_view k_true_digit  = "1";
inline constexpr std::string_view k_true_short  = "y";
inline constexpr std::string_view k_true_yes    = "yes";
inline constexpr std::string_view k_true_word   = "true";
inline constexpr std::string_view k_false_digit = "0";
inline constexpr std::string_view k_false_short = "n";
inline constexpr std::string_view k_false_no    = "no";
inline constexpr std::string_view k_false_word  = "false";

// ---- Type label for boolean -----------------------------------------------

/// Used as the target_type label in ConversionFailure for parse_bool().
inline constexpr std::string_view k_type_bool = "bool";

// ---- spdlog format templates ---------------------------------------------

/// ATE_LOG_ERROR format — parse_or_throw() when conversion fails completely.
inline constexpr std::string_view k_fmt_parse_error =
    "safe_conversion: failed to parse '{}' as {} (field='{}', record='{}')";

/// ATE_LOG_DEBUG format — parse_or_default() / parse_bool_or_default() when field is empty.
inline constexpr std::string_view k_fmt_empty_field =
    "safe_conversion: empty '{}' (record='{}'), using default";

/// ATE_LOG_WARN format — parse_or_default() when value fails conversion.
inline constexpr std::string_view k_fmt_invalid =
    "safe_conversion: invalid '{}' (record='{}', raw='{}'), using default";

/// ATE_LOG_WARN format — parse_bool_or_default() when bool value is invalid.
inline constexpr std::string_view k_fmt_invalid_bool =
    "safe_conversion: invalid bool '{}' (record='{}', raw='{}'), using default";

} // namespace ate::conv_msg
