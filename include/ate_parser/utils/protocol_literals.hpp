/**
 * @file protocol_literals.hpp
 * @brief Constexpr string constants for ATE log protocol structural markers.
 *
 * Only strings that have NO direct `Prefix` enum equivalent belong here.
 * For any \@prefix string that is already represented by `enums::Prefix`,
 * use `enums::to_string_view(Prefix::xxx)` at the call site instead.
 */
#pragma once

#include <string_view>

namespace ate::proto {

// ---- Analog test group marker ---------------------------------------------

/// Common leading substring shared by all analog test records (@A-*).
/// There is no single `Prefix` enum value for the group — each variant
/// (a_cap, a_dio, …) has its own, so this prefix is used only for
/// fast detection in `is_analog_test_text()`.
inline constexpr std::string_view k_analog_group = "@A-";

// ---- Batch structural anchor ----------------------------------------------

/// Search anchor for locating the opening @BATCH record brace.
/// Structurally equivalent to '{' + to_string_view(Prefix::batch).
/// Kept as a literal because to_string_view is not constexpr and
/// string_view concatenation cannot be done at compile time.
inline constexpr std::string_view k_batch_open_brace = "{@BATCH";

// ---- Diagnostic context labels -------------------------------------------

/// Logical grouping label used as record_type context in parse_or_default
/// calls for analog test records. There is no single @ANALOG enum value;
/// all @A-* variants share this diagnostic label.
inline constexpr std::string_view k_analog_context = "@ANALOG";

} // namespace ate::proto
