/**
 * @file ShortRecords.cpp
 * @brief Implements parsing and JSON serialization for shorts test records
 *
 * This source file defines:
 *  - toJson() functions to convert record objects to JSON format
 *  - A helper splitFields() to handle normal, list (\N), and literal (~) fields
 *
 * Based strictly on Keysight i3070 Log Record Format (Tables 8-47 to 8-51) [1].
 */

#include "keysight_log/records/ShortRecords.hpp"
#include "keysight_log/utils/SafeConversion.hpp"
#include "keysight_log/utils/JsonKeys.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace keysight_log {
namespace records {

// ========================= ShortsTestRecord =========================

void ShortsTestRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) test_status = static_cast<enums::GenericTestStatus>(core::safeStoi(fields[0], "test_status", "@TS", 0));
    if (fields.size() > 1) shorts_count = core::safeStoi(fields[1], "shorts_count", "@TS", 0);
    if (fields.size() > 2) opens_count = core::safeStoi(fields[2], "opens_count", "@TS", 0);
    if (fields.size() > 3) phantoms_count = core::safeStoi(fields[3], "phantoms_count", "@TS", 0);
    if (fields.size() > 4) designator = fields[4];
    designator.erase(remove(designator.begin(), designator.end(), '\r'), designator.end());
    designator.erase(remove(designator.begin(), designator.end(), '\n'), designator.end());
}

nlohmann::json ShortsTestRecord::toJson() const {
    nlohmann::json j = LogRecord::toJson();
    j[JSON_KEY_TEST_STATUS] = static_cast<int>(test_status);
    j[JSON_KEY_SHORTS_COUNT] = shorts_count;
    j[JSON_KEY_OPENS_COUNT] = opens_count;
    j[JSON_KEY_PHANTOMS_COUNT] = phantoms_count;
    j[JSON_KEY_DESIGNATOR] = designator == "" ? json(nullptr) : json(designator);
    return j;
}

// ========================= TsSourceRecord =========================

void TsSourceRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) shorts_count = core::safeStoi(fields[0], "shorts_count", "@TS-S", 0);
    if (fields.size() > 1) phantoms_count = core::safeStoi(fields[1], "phantoms_count", "@TS-S", 0);
    if (fields.size() > 2) source_node = fields[2];
    source_node.erase(remove(source_node.begin(), source_node.end(), '\r'), source_node.end());
    source_node.erase(remove(source_node.begin(), source_node.end(), '\n'), source_node.end());
}

nlohmann::json TsSourceRecord::toJson() const {
    nlohmann::json j = LogRecord::toJson();
    j[JSON_KEY_SHORTS_COUNT] = shorts_count;
    j[JSON_KEY_PHANTOMS_COUNT] = phantoms_count;
    j[JSON_KEY_SOURCE_NODE] = source_node;
    return j;
}

// ====================== TsDestinationRecord ======================

void TsDestinationRecord::fromFields(const vector<string>& fields) {
    destination_list.clear();
    for (size_t i = 1; i + 1 < fields.size(); i += 2) {
        destination_list.emplace_back(fields[i], core::safeStod(fields[i + 1], "deviation", "@TS-D", 0.0));
    }
}

nlohmann::json TsDestinationRecord::toJson() const {
    nlohmann::json arr = nlohmann::json::array();
    for (auto& p : destination_list) {
        arr.push_back({p.first, p.second});
    }
    nlohmann::json j = LogRecord::toJson();
    j[JSON_KEY_DESTINATIONS_LIST] = arr;
    return j;
}

// ========================= TsOpenRecord =========================

void TsOpenRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) source_node = fields[0];
    if (fields.size() > 1) destination_node = fields[1];
    if (fields.size() > 2) deviation = core::safeStodOptional(fields[2], "deviation", "@TS-O");
}

nlohmann::json TsOpenRecord::toJson() const {
    nlohmann::json j = LogRecord::toJson();
    j[JSON_KEY_SOURCE_NODE] = source_node;
    j[JSON_KEY_DESTINATION_NODE] = destination_node;
    j[JSON_KEY_DEVIATION] = deviation.has_value() ? nlohmann::json(*deviation) : nlohmann::json(nullptr);
    return j;
}

// ======================= TsPhantomRecord =======================

void TsPhantomRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) deviation = core::safeStodOptional(fields[0], "deviation", "@TS-P");
}

nlohmann::json TsPhantomRecord::toJson() const {
    nlohmann::json j = LogRecord::toJson();
    j[JSON_KEY_DEVIATION] = deviation.has_value() ? nlohmann::json(*deviation) : nlohmann::json(nullptr);
    return j;
}

} // namespace records
} // namespace keysight_log
