/**
 * @file error_messages.hpp
 * @brief Constexpr string constants for parser exception messages.
 *
 * Single source of truth for all strings passed to `MalformedRecordError`
 * and `IntegrityError` constructors inside the core parser. The one partially
 * dynamic message (@RPT literal length) uses `k_rpt_invalid_length_prefix`
 * as its leading text, concatenated with the runtime value at throw site.
 */
#pragma once

#include <string_view>

namespace ate::err_msg {

// ---- @RPT record errors ---------------------------------------------------

inline constexpr std::string_view k_rpt_missing_pipe         = "Invalid @RPT format: missing | after literal length";
inline constexpr std::string_view k_rpt_invalid_length_prefix = "Invalid @RPT literal length: ";
inline constexpr std::string_view k_rpt_exceeds_size         = "Invalid @RPT format: literal exceeds record size";

// ---- @PIN record errors ---------------------------------------------------

inline constexpr std::string_view k_pin_missing_pipe = "Invalid @PIN format: missing | after count";

// ---- @TS-D record errors --------------------------------------------------

inline constexpr std::string_view k_tsd_missing_pipe = "Invalid @TS-D format: missing | after count";

// ---- General structure errors ---------------------------------------------

inline constexpr std::string_view k_unbalanced_braces = "Unbalanced braces in record stream";

// ---- Integrity check errors -----------------------------------------------

inline constexpr std::string_view k_empty_log       = "empty log content";
inline constexpr std::string_view k_no_batch_found  = "failed integrity: no @BATCH record found";
inline constexpr std::string_view k_first_not_batch = "failed integrity: first record is not @BATCH";

} // namespace ate::err_msg
