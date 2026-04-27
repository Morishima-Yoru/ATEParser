#include "ate_parser/enums/field_type.hpp"
#include "ate_parser/utils/safe_conversion.hpp"
#include <algorithm>
#include <cctype>
#include <cstdio>

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
    if (lower == "bool" || lower == "boolean") return FieldType::boolean;
    if (lower == "fp" || lower == "float" || lower == "double") return FieldType::fp;
    if (lower == "int" || lower == "integer") return FieldType::integer;
    if (lower == "str" || lower == "string") return FieldType::str;
    if (lower == "list" || lower == "array") return FieldType::list;
    if (lower == "literal" || lower == "raw") return FieldType::literal;
    return FieldType::unknown;
}

std::string to_string(FieldType t) {
    switch (t) {
        case FieldType::boolean: return "bool";
        case FieldType::fp:      return "fp";
        case FieldType::integer: return "int";
        case FieldType::str:     return "str";
        case FieldType::list:    return "list";
        case FieldType::literal: return "literal";
        case FieldType::unknown: return "unknown";
    }
    return "unknown";
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
