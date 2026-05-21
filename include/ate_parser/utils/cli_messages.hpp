/**
 * @file cli_messages.hpp
 * @brief Constexpr string constants for the ATEParserApp command-line interface.
 *
 * Single source of truth for all text emitted by the command-line entry point.
 * Constants are grouped by category and exposed as `inline constexpr std::string_view`
 * so they are compile-time constants with no ODR issues.
 */
#pragma once

#include <string_view>

namespace ate::cli_msg {

// ---- Argument flag literals -----------------------------------------------

inline constexpr std::string_view k_arg_raw    = "--raw";
inline constexpr std::string_view k_arg_debug  = "--debug";
inline constexpr std::string_view k_arg_help   = "--help";
inline constexpr std::string_view k_arg_indent = "--indent";
inline constexpr std::string_view k_arg_output = "--output";

// ---- Usage invocation line ------------------------------------------------

inline constexpr std::string_view k_usage_invocation_prefix = "Usage: ";
inline constexpr std::string_view k_usage_invocation_args   = " [options] <logfile>\n";

// ---- Usage block ----------------------------------------------------------

inline constexpr std::string_view k_usage_header = "Options:\n";
inline constexpr std::string_view k_usage_raw    = "  --raw            Keep raw flat-field text in JSON output\n";
inline constexpr std::string_view k_usage_indent = "  --indent <N>     JSON indentation size (default: 2)\n";
inline constexpr std::string_view k_usage_output = "  --output <path>  Output JSON file path\n";
inline constexpr std::string_view k_usage_debug  = "  --debug          Verbose logging\n";
inline constexpr std::string_view k_usage_help   = "  --help           Show this help message\n";

// ---- CLI argument error messages ------------------------------------------

inline constexpr std::string_view k_err_missing_indent        = "Missing value for --indent\n";
inline constexpr std::string_view k_err_invalid_indent        = "Invalid --indent value\n";
inline constexpr std::string_view k_err_missing_output        = "Missing value for --output\n";
inline constexpr std::string_view k_err_unknown_arg           = "Unknown argument: ";
inline constexpr std::string_view k_err_unexpected_positional = "Unexpected positional argument: ";
inline constexpr std::string_view k_err_no_input              = "No input file specified.\n";

// ---- I/O error messages ---------------------------------------------------

inline constexpr std::string_view k_err_open_log    = "Cannot open log file: ";
inline constexpr std::string_view k_err_open_output = "Cannot open output file: ";

// ---- Success messages -----------------------------------------------------

inline constexpr std::string_view k_msg_converted_prefix = "Successfully converted ";
inline constexpr std::string_view k_msg_converted_to     = " to ";

// ---- Exception prefix messages --------------------------------------------

inline constexpr std::string_view k_err_parse_error = "ParseError: ";
inline constexpr std::string_view k_err_generic     = "Error: ";
inline constexpr std::string_view k_err_unknown     = "Unknown error.\n";

} // namespace ate::cli_msg
