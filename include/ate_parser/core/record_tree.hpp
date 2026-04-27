/**
 * @file record_tree.hpp
 * @brief Hierarchical tree of records (replaces LogRecordContainer).
 *
 * A `RecordNode` owns a `Record` and zero or more child `RecordNode`s.
 * Children represent nested subrecords (the brace-grouped hierarchy of
 * the i3070 log format). All children are stored by value in a vector --
 * no `unique_ptr` indirection.
 */
#pragma once

#include "ate_parser/core/record.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace ate::core {

struct RecordNode {
    Record                  record;       ///< This node's record (monostate for the synthetic root).
    std::string             raw_prefix;   ///< Prefix as it appeared in the source text (e.g. "@A-CAP").
    std::string             raw_data;     ///< Full flat-field text, kept when `keep_raw == true`.
    std::vector<RecordNode> children;
};

/// Serialize @p node to JSON. Children are emitted under the
/// `json_keys::subrecords` key. If @p keep_raw is true, the original raw
/// flat-field text is added under `json_keys::raw`.
[[nodiscard]] nlohmann::json to_json(const RecordNode& node, bool keep_raw = false);

} // namespace ate::core
