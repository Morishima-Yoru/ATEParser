#include "ate_parser/utils/safe_conversion.hpp"
#include "ate_parser/utils/errors.hpp"
#include "ate_parser/utils/logging.hpp"
#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <string>
#include <system_error>
#include <typeinfo>

namespace ate::utils {

namespace {

template <typename T>
constexpr std::string_view type_name_of() {
    if constexpr (std::is_same_v<T, int>)                return "int";
    else if constexpr (std::is_same_v<T, long>)          return "long";
    else if constexpr (std::is_same_v<T, long long>)     return "long long";
    else if constexpr (std::is_same_v<T, unsigned>)      return "unsigned";
    else if constexpr (std::is_same_v<T, unsigned long>) return "unsigned long";
    else if constexpr (std::is_same_v<T, unsigned long long>) return "unsigned long long";
    else if constexpr (std::is_same_v<T, float>)         return "float";
    else if constexpr (std::is_same_v<T, double>)        return "double";
    else                                                 return "unknown";
}

std::string_view trim(std::string_view sv) noexcept {
    auto is_ws = [](unsigned char c) { return std::isspace(c); };
    while (!sv.empty() && is_ws(static_cast<unsigned char>(sv.front()))) sv.remove_prefix(1);
    while (!sv.empty() && is_ws(static_cast<unsigned char>(sv.back())))  sv.remove_suffix(1);
    return sv;
}

template <typename T>
std::expected<T, ConversionFailure> parse_numeric(std::string_view sv) {
    sv = trim(sv);
    if (sv.empty()) {
        return std::unexpected(ConversionFailure{
            ConversionErrc::empty, std::string{sv}, std::string{type_name_of<T>()}});
    }
    // std::from_chars rejects a leading '+' for numeric types; normalize it.
    if (sv.size() > 1 && sv.front() == '+') {
        sv.remove_prefix(1);
    }
    T value{};
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (ec == std::errc::invalid_argument || ptr != sv.data() + sv.size()) {
        return std::unexpected(ConversionFailure{
            ConversionErrc::invalid_syntax, std::string{sv}, std::string{type_name_of<T>()}});
    }
    if (ec == std::errc::result_out_of_range) {
        return std::unexpected(ConversionFailure{
            ConversionErrc::out_of_range, std::string{sv}, std::string{type_name_of<T>()}});
    }
    return value;
}

} // namespace

template <typename T>
std::expected<T, ConversionFailure> parse_to(std::string_view str) {
    return parse_numeric<T>(str);
}

// Explicit instantiations
template std::expected<int,                ConversionFailure> parse_to<int>(std::string_view);
template std::expected<long,               ConversionFailure> parse_to<long>(std::string_view);
template std::expected<long long,          ConversionFailure> parse_to<long long>(std::string_view);
template std::expected<unsigned,           ConversionFailure> parse_to<unsigned>(std::string_view);
template std::expected<unsigned long,      ConversionFailure> parse_to<unsigned long>(std::string_view);
template std::expected<unsigned long long, ConversionFailure> parse_to<unsigned long long>(std::string_view);
template std::expected<float,              ConversionFailure> parse_to<float>(std::string_view);
template std::expected<double,             ConversionFailure> parse_to<double>(std::string_view);

std::expected<bool, ConversionFailure> parse_bool(std::string_view str) {
    auto sv = trim(str);
    if (sv.empty()) {
        return std::unexpected(ConversionFailure{ConversionErrc::empty, std::string{sv}, "bool"});
    }
    auto iequals = [](std::string_view a, std::string_view b) noexcept {
        if (a.size() != b.size()) return false;
        for (std::size_t i = 0; i < a.size(); ++i)
            if (std::tolower(static_cast<unsigned char>(a[i])) !=
                std::tolower(static_cast<unsigned char>(b[i]))) return false;
        return true;
    };
    if (sv == "1" || iequals(sv, "y") || iequals(sv, "yes") || iequals(sv, "true"))  return true;
    if (sv == "0" || iequals(sv, "n") || iequals(sv, "no")  || iequals(sv, "false")) return false;
    return std::unexpected(ConversionFailure{
        ConversionErrc::invalid_syntax, std::string{sv}, "bool"});
}

template <typename T>
T parse_or_throw(std::string_view str,
                 std::string_view field_name,
                 std::string_view record_type) {
    auto r = parse_to<T>(str);
    if (r) return *r;
    ATE_LOG_ERROR("safe_conversion: failed to parse '{}' as {} (field='{}', record='{}')",
                  std::string{str}, std::string{type_name_of<T>()},
                  std::string{field_name}, std::string{record_type});
    throw ConversionError(str, type_name_of<T>(), field_name, record_type);
}

template int                parse_or_throw<int>(std::string_view, std::string_view, std::string_view);
template long               parse_or_throw<long>(std::string_view, std::string_view, std::string_view);
template long long          parse_or_throw<long long>(std::string_view, std::string_view, std::string_view);
template unsigned           parse_or_throw<unsigned>(std::string_view, std::string_view, std::string_view);
template unsigned long      parse_or_throw<unsigned long>(std::string_view, std::string_view, std::string_view);
template unsigned long long parse_or_throw<unsigned long long>(std::string_view, std::string_view, std::string_view);
template float              parse_or_throw<float>(std::string_view, std::string_view, std::string_view);
template double             parse_or_throw<double>(std::string_view, std::string_view, std::string_view);

template <typename T>
T parse_or_default(std::string_view str,
                   std::string_view field_name,
                   std::string_view record_type,
                   T default_value) {
    auto sv = trim(str);
    if (sv.empty()) {
        ATE_LOG_DEBUG("safe_conversion: empty '{}' (record='{}'), using default",
                      std::string{field_name}, std::string{record_type});
        return default_value;
    }
    auto r = parse_to<T>(sv);
    if (r) return *r;
    ATE_LOG_WARN("safe_conversion: invalid '{}' (record='{}', raw='{}'), using default",
                 std::string{field_name}, std::string{record_type}, std::string{sv});
    return default_value;
}

template int                parse_or_default<int>(std::string_view, std::string_view, std::string_view, int);
template long               parse_or_default<long>(std::string_view, std::string_view, std::string_view, long);
template long long          parse_or_default<long long>(std::string_view, std::string_view, std::string_view, long long);
template unsigned           parse_or_default<unsigned>(std::string_view, std::string_view, std::string_view, unsigned);
template unsigned long      parse_or_default<unsigned long>(std::string_view, std::string_view, std::string_view, unsigned long);
template unsigned long long parse_or_default<unsigned long long>(std::string_view, std::string_view, std::string_view, unsigned long long);
template float              parse_or_default<float>(std::string_view, std::string_view, std::string_view, float);
template double             parse_or_default<double>(std::string_view, std::string_view, std::string_view, double);

bool parse_bool_or_default(std::string_view str,
                           std::string_view field_name,
                           std::string_view record_type,
                           bool default_value) {
    auto sv = trim(str);
    if (sv.empty()) {
        ATE_LOG_DEBUG("safe_conversion: empty '{}' (record='{}'), using default",
                      std::string{field_name}, std::string{record_type});
        return default_value;
    }
    auto r = parse_bool(sv);
    if (r) return *r;
    ATE_LOG_WARN("safe_conversion: invalid bool '{}' (record='{}', raw='{}'), using default",
                 std::string{field_name}, std::string{record_type}, std::string{sv});
    return default_value;
}

} // namespace ate::utils
