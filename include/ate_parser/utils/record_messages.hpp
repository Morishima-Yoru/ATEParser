/**
 * @file record_messages.hpp
 * @brief Constexpr string constants for record-level parser diagnostics.
 *
 * Centralises the warning template emitted during @A-* record parsing and
 * the exception messages thrown by analog record helper functions.
 * For parser structural and integrity error messages, see error_messages.hpp.
 */
#pragma once

#include <string_view>

namespace ate::rec_msg {

// ---- @ANALOG record warnings ---------------------------------------------

/// spdlog format template used in ATE_LOG_WARN when an unknown
/// AnalogTestStatus integer code is encountered during record parsing.
inline constexpr std::string_view k_analog_unknown_status = "@ANALOG: unknown test_status code {}";

// ---- Lim3Record::percent_deviation ---------------------------------------

/// Thrown as std::domain_error when nominal_value is zero.
inline constexpr std::string_view k_lim3_zero_nominal = "Lim3Record::percent_deviation: nominal_value is zero";

// ---- expects_lim3 guard --------------------------------------------------

/// Thrown as std::invalid_argument when called with a non-analog prefix.
inline constexpr std::string_view k_expects_lim3_bad_prefix = "expects_lim3: not an analog test prefix";

} // namespace ate::rec_msg
