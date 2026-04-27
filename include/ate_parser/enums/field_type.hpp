/**
 * @file field_type.hpp
 * @brief Field type tag and primitive-value variant used inside records.
 */
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace ate::enums {

enum class FieldType { boolean, fp, integer, str, list, literal, unknown };

/// A primitive value carried by a single field.
using FieldValue = std::variant<bool, int, double, std::string>;

[[nodiscard]] FieldType   to_field_type(std::string_view s) noexcept;
[[nodiscard]] std::string to_string(FieldType t);

[[nodiscard]] std::optional<FieldValue> parse_field_value(std::string_view s, FieldType target);

[[nodiscard]] std::string field_value_to_string(const FieldValue& v);
[[nodiscard]] FieldType   type_of(const FieldValue& v);
[[nodiscard]] char        type_tag(const FieldValue& v);  // 'b', 'i', 'f', 's'

} // namespace ate::enums
