#include "ate_parser/core/record_tree.hpp"
#include "ate_parser/utils/json_keys.hpp"

namespace ate::core {

nlohmann::json to_json(const RecordNode& node, bool keep_raw) {
    nlohmann::json j = is_unknown(node.record) ? nlohmann::json::object() : to_json(node.record);

    if (keep_raw && !node.raw_data.empty()) {
        j[std::string{json_keys::raw}] = node.raw_data;
    }
    if (!node.children.empty()) {
        nlohmann::json arr = nlohmann::json::array();
        arr.get_ref<nlohmann::json::array_t&>().reserve(node.children.size());
        for (const auto& c : node.children) arr.push_back(to_json(c, keep_raw));
        j[std::string{json_keys::subrecords}] = std::move(arr);
    }
    return j;
}

} // namespace ate::core
