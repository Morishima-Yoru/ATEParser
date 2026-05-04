#include "ate_parser/enums/field_type.hpp"
#include "ate_parser/utils/enum_messages.hpp"
#include "ate_parser/utils/safe_conversion.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace em = ate::enum_msg;

namespace ate::enums {

namespace {
std::string to_lower(std::string_view sv) {
    std::string out;
    out.reserve(sv.size());
    for (char c : sv) out.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    return out;
}
} // namespace

FieldType to_field_type(std::string_view s) noexcept {
    auto lower = to_lower(s);
    if (lower == em::k_field_bool    || lower == em::k_token_boolean) return FieldType::boolean;
    if (lower == em::k_field_fp      || lower == em::k_token_float
                                     || lower == em::k_token_double)  return FieldType::fp;
    if (lower == em::k_field_int     || lower == em::k_token_integer) return FieldType::integer;
    if (lower == em::k_field_str     || lower == em::k_token_string)  return FieldType::str;
    if (lower == em::k_field_list    || lower == em::k_token_array)   return FieldType::list;
    if (lower == em::k_field_literal || lower == em::k_token_raw)     return FieldType::literal;
    return FieldType::unknown;
}

std::string to_string(FieldType t) {
    switch (t) {
        case FieldType::boolean: return std::string{em::k_field_bool};
        case FieldType::fp:      return std::string{em::k_field_fp};
        case FieldType::integer: return std::string{em::k_field_int};
        case FieldType::str:     return std::string{em::k_field_str};
        case FieldType::list:    return std::string{em::k_field_list};
        case FieldType::literal: return std::string{em::k_field_literal};
        case FieldType::unknown: return std::string{em::k_field_unknown};
    }
    return std::string{em::k_field_unknown};
}

std::optional<FieldValue> parse_field_value(std::string_view s, FieldType target) {
    switch (target) {
        case FieldType::boolean: {
            auto r = utils::parse_bool(s);
            if (r) return FieldValue{*r};
            return std::nullopt;
        }
        case FieldType::integer: {
            auto r = utils::parse_to<int>(s);
            if (r) return FieldValue{*r};
            return std::nullopt;
        }
        case FieldType::fp: {
            auto r = utils::parse_to<double>(s);
            if (r) return FieldValue{*r};
            return std::nullopt;
        }
        case FieldType::str:
            return FieldValue{std::string{s}};
        default:
            return std::nullopt;
    }
}

std::string field_value_to_string(const FieldValue& v) {
    if (auto* p = std::get_if<bool>(&v))         return *p ? "1" : "0";
    if (auto* p = std::get_if<int>(&v))          return std::to_string(*p);
    if (auto* p = std::get_if<double>(&v)) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%.6E", *p);
        return std::string(buf);
    }
    if (auto* p = std::get_if<std::string>(&v))  return *p;
    return {};
}

FieldType type_of(const FieldValue& v) {
    if (std::holds_alternative<bool>(v))        return FieldType::boolean;
    if (std::holds_alternative<int>(v))         return FieldType::integer;
    if (std::holds_alternative<double>(v))      return FieldType::fp;
    if (std::holds_alternative<std::string>(v)) return FieldType::str;
    return FieldType::unknown;
}

char type_tag(const FieldValue& v) {
    switch (type_of(v)) {
        case FieldType::boolean: return 'b';
        case FieldType::integer: return 'i';
        case FieldType::fp:      return 'f';
        default:                 return 's';
    }
}

} // namespace ate::enums
