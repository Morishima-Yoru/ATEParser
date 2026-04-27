/**
 * @file safe_conversion.hpp
 * @brief Modern, exception-light string -> value conversions.
 *
 *  - Numeric parsing uses std::from_chars (no allocation, no exceptions,
 *    locale-independent).
 *  - Public API returns std::expected<T, ConversionFailure> (C++23). The
 *    caller decides whether to log, default, or throw.
 *  - The throwing variant `parse_or_throw<T>(...)` raises a traceable
 *    `ate::ConversionError`.
 */
#pragma once

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

namespace ate::utils {

enum class ConversionErrc {
    empty,
    invalid_syntax,
    out_of_range,
    unsupported,
};

struct ConversionFailure {
    ConversionErrc errc;
    std::string    raw;
    std::string    target_type;
};

/**
 * Parse @p str as a value of type @p T using std::from_chars.
 * Whitespace at both ends is trimmed.
 *
 * Supported T: int, long, long long, unsigned, unsigned long,
 * unsigned long long, std::int64_t, std::uint64_t, float, double.
 */
template <typename T>
[[nodiscard]] std::expected<T, ConversionFailure> parse_to(std::string_view str);

/// Permissive boolean parser: "1/0", "Y/N", "yes/no", "true/false" (any case).
[[nodiscard]] std::expected<bool, ConversionFailure> parse_bool(std::string_view str);

/**
 * Throwing wrapper around `parse_to`. Logs and raises `ate::ConversionError`
 * on failure. @p field_name and @p record_type are used purely for the
 * diagnostic message; pass empty views if not relevant.
 */
template <typename T>
T parse_or_throw(std::string_view str,
                 std::string_view field_name,
                 std::string_view record_type);

/**
 * Like `parse_or_throw` but tolerates empty input by returning the
 * provided default value (with an info-level log entry).
 */
template <typename T>
T parse_or_default(std::string_view str,
                   std::string_view field_name,
                   std::string_view record_type,
                   T default_value);

/// Same shape as `parse_or_default` but for booleans.
bool parse_bool_or_default(std::string_view str,
                           std::string_view field_name,
                           std::string_view record_type,
                           bool default_value);

} // namespace ate::utils
