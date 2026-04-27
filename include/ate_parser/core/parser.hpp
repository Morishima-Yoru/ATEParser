/**
 * @file parser.hpp
 * @brief The top-level i3070 log parser.
 */
#pragma once

#include "ate_parser/core/record_tree.hpp"
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ate::core {

/// Result of splitting a single record's flat-field text into prefix +
/// individual field strings.
struct ParsedFields {
    std::string                   prefix;
    std::vector<std::string_view> fields;
};

/// Stateless interface for parsing i3070 ICT log content.
class Parser {
public:
    /**
     * Parse a complete log document. The returned root node is a synthetic
     * container whose first child is always the @BATCH record.
     *
     * Throws:
     *  - `ate::IntegrityError` if the input does not contain a top-level
     *    @BATCH record (the i3070 log integrity contract).
     *  - `ate::MalformedRecordError` if a record's braces are unbalanced.
     */
    [[nodiscard]] RecordNode parse(std::string_view log_text, bool keep_raw = false) const;

    /// Split a single flat record (no enclosing braces) into prefix + fields.
    [[nodiscard]] static ParsedFields parse_fields(std::string_view record_text);
};

// ---- C ABI for embedders (Python ctypes, .NET P/Invoke, ...) ----------------

enum class ParseResult : int {
    success                 = 0,
    input_error             = 1,
    parse_error             = 2,
    json_conversion_error   = 3,
    output_error            = 4,
    failed_integrity        = 5,
};

#ifdef _WIN32
#  define ATE_API __declspec(dllexport)
#else
#  define ATE_API
#endif

extern "C" ATE_API int ate_parse_file        (const char* log_filepath, const char* dst_filepath, int indent, int keep_raw, char** out_json_str);
extern "C" ATE_API int ate_parse_log_string  (const char* log_content,  const char* dst_filepath, int indent, int keep_raw, char** out_json_str);
extern "C" ATE_API void ate_free_string      (char* ptr);

} // namespace ate::core
