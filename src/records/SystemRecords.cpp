/**
 * @file SystemRecords.cpp
 * @brief Implements system-level and batch record parsing and utilities
 *
 * This source file provides:
 *  - Utility methods for JSON serialization
 *
 * Based strictly on Keysight i3070 Log Record Format (Tables 8-21 to 8-27) [1].
 */

#include "keysight_log/records/SystemRecords.hpp"
#include "keysight_log/utils/SafeConversion.hpp"
#include "keysight_log/utils/JsonKeys.hpp"
#include "keysight_log/core/FieldValue.hpp"
#include <nlohmann/json.hpp>

namespace keysight_log {
namespace records {

using namespace std;
using json = nlohmann::json;
using core::FieldValue;


// ===================== BatchRecord =====================

void BatchRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) uut_type = fields[0];
    if (fields.size() > 1) uut_type_rev = fields[1];
    if (fields.size() > 2) fixture_id = core::safeStoi(fields[2], "fixture_id", "@BATCH", 0);
    if (fields.size() > 3) testhead_number = core::safeStoi(fields[3], "testhead_number", "@BATCH", 1);
    if (fields.size() > 4) testhead_type = fields[4];
    if (fields.size() > 5) process_step = fields[5];
    if (fields.size() > 6) batch_id = fields[6];
    if (fields.size() > 7) operator_id = fields[7];
    if (fields.size() > 8) controller = fields[8];
    if (fields.size() > 9) testplan_id = fields[9];
    if (fields.size() > 10) testplan_rev = fields[10];
    if (fields.size() > 11) parent_panel_type = fields[11];
    if (fields.size() > 12) parent_panel_type_rev = fields[12];
    if (fields.size() > 13) version_label = fields[13];
    version_label.erase(remove(version_label.begin(), version_label.end(), '\r'), version_label.end());
    version_label.erase(remove(version_label.begin(), version_label.end(), '\n'), version_label.end());
}

json BatchRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_UUT_TYPE]              = uut_type;
    j[JSON_KEY_UUT_TYPE_REV]          = uut_type_rev;
    j[JSON_KEY_FIXTURE_ID]            = fixture_id;
    j[JSON_KEY_TESTHEAD_NUMBER]       = testhead_number;
    j[JSON_KEY_TESTHEAD_TYPE]         = testhead_type;
    j[JSON_KEY_PROCESS_STEP]          = process_step;
    j[JSON_KEY_BATCH_ID]              = batch_id;
    j[JSON_KEY_OPERATOR_ID]           = operator_id;
    j[JSON_KEY_CONTROLLER]            = controller;
    j[JSON_KEY_TESTPLAN_ID]           = testplan_id;
    j[JSON_KEY_TESTPLAN_REV]          = testplan_rev;
    j[JSON_KEY_PARENT_PANEL_TYPE]     = parent_panel_type;
    j[JSON_KEY_PARENT_PANEL_TYPE_REV] = parent_panel_type_rev;
    j[JSON_KEY_VERSION_LABEL]         = version_label;
    return j;
}

// ===================== BlockRecord =====================

void BlockRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) block_designator = fields[0];
    if (fields.size() > 1) block_status = core::safeStoi(fields[1], "block_status", "@BLOCK", 0);
}

json BlockRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_BLOCK_DESIGNATOR] = block_designator;
    j[JSON_KEY_BLOCK_STATUS] = block_status;
    return j;
}

// ===================== AlarmRecord =====================

void AlarmRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) alarm_type = core::safeStoi(fields[0], "alarm_type", "@ALM", 1);
    if (fields.size() > 1) alarm_status = (core::safeStoi(fields[1], "alarm_status", "@ALM", 0) != 0);
    if (fields.size() > 2) datetime = fields[2];
    if (fields.size() > 3) board_type = fields[3];
    if (fields.size() > 4) board_rev = fields[4];
    if (fields.size() > 5) alarm_limit = core::safeStoiOptional(fields[5], "alarm_limit", "@ALM");
    if (fields.size() > 6) detected_value = core::safeStoiOptional(fields[6], "detected_value", "@ALM");
    if (fields.size() > 7) controller = fields[7];
    if (fields.size() > 8) testhead_number = core::safeStoi(fields[8], "testhead_number", "@ALM", 1);
}

json AlarmRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_ALARM_TYPE] = alarm_type;
    j[JSON_KEY_ALARM_STATUS] = alarm_status;
    j[JSON_KEY_DATETIME] = datetime;
    j[JSON_KEY_BOARD_TYPE] = board_type;
    j[JSON_KEY_BOARD_REV] = board_rev;
    j[JSON_KEY_ALARM_LIMIT] = alarm_limit.has_value() ? json(*alarm_limit) : json(nullptr);
    j[JSON_KEY_DETECTED_VALUE] = detected_value.has_value() ? json(*detected_value) : json(nullptr);
    j[JSON_KEY_CONTROLLER] = controller;
    j[JSON_KEY_TESTHEAD_NUMBER] = testhead_number;
    return j;
}

// ===================== AlarmBoardRecord =====================

void AlarmBoardRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) datetime = fields[0];
    if (fields.size() > 1) serial = fields[1];
    serial.erase(remove(serial.begin(), serial.end(), '\r'), serial.end());
    serial.erase(remove(serial.begin(), serial.end(), '\n'), serial.end());
}

json AlarmBoardRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_DATETIME] = datetime;
    j[JSON_KEY_SERIAL_NUMBER] = serial;
    return j;
}

// ===================== ArrayRecord =====================

void ArrayRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) designator = fields[0];
    if (fields.size() > 1) status = core::safeStoi(fields[1], "status", "@ARRAY", 0);
    if (fields.size() > 2) failure_count = core::safeStoiOptional(fields[2], "failure_count", "@ARRAY");
    if (fields.size() > 3) samples = core::safeStoiOptional(fields[3], "samples", "@ARRAY");
}

json ArrayRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_DESIGNATOR] = designator;
    j[JSON_KEY_STATUS] = status;
    j[JSON_KEY_FAILURE_COUNT] = failure_count.has_value() ? json(*failure_count) : json(nullptr);
    j[JSON_KEY_SAMPLES] = samples.has_value() ? json(*samples) : json(nullptr);
    return j;
}

// ===================== NetVerifyRecord =====================

void NetVerifyRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) datetime = fields[0];
    if (fields.size() > 1) test_system = fields[1];
    if (fields.size() > 2) repair_system = fields[2];
    if (fields.size() > 3) source = (core::safeStoi(fields[3], "source", "@NETV", 0) != 0);
}

json NetVerifyRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_DATETIME] = datetime;
    j[JSON_KEY_TEST_SYSTEM] = test_system;
    j[JSON_KEY_REPAIR_SYSTEM] = repair_system;
    j[JSON_KEY_SOURCE] = source;
    return j;
}

// ===================== NodeListRecord =====================

void NodeListRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) count = core::safeStoi(fields[0], "count", "@NODE", 0);
    if (fields.size() > 1) {
        nodes.clear();
        for (size_t i = 1; i < fields.size(); ++i) {
            nodes.push_back(fields[i]);
        }
    }
}

json NodeListRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_COUNT] = count;
    j[JSON_KEY_NODES] = nodes;
    return j;
}

// ===================== ReportRecord =====================

void ReportRecord::fromFields(const vector<string>& fields) {
    message.clear();
    for (size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) message += "|";
        message += fields[i];
    }
}

json ReportRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_MESSAGE] = message;
    return j;
}

// ===================== RetestRecord =====================

void RetestRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) datetime = fields[0];
    datetime.erase(remove(datetime.begin(), datetime.end(), '\r'), datetime.end());
    datetime.erase(remove(datetime.begin(), datetime.end(), '\n'), datetime.end());
}

json RetestRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_DATETIME] = datetime;
    return j;
}

// ===================== PFRecord =====================
void PFRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) designator = fields[0];
    if (fields.size() > 1) test_status = core::safeStoiOptional(fields[1], "test_status", "@PF");
    if (fields.size() > 2) total_pins = core::safeStoiOptional(fields[2], "total_pins", "@PF");
}
json PFRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_DESIGNATOR] = designator;
    j[JSON_KEY_TEST_STATUS] = test_status.has_value() ? json(*test_status) : json(nullptr);
    j[JSON_KEY_TOTAL_PINS] = total_pins.has_value() ? json(*total_pins) : json(nullptr);
    return j;
}

// ===================== BsShortRecord =====================
void BsShortRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) {
        // The format of the first field: "S\\count"
        string first_field = fields[0];
        size_t backslash_pos = first_field.find('\\');
        if (backslash_pos != string::npos) {
            cause = first_field.substr(0, backslash_pos);
            string count_str = first_field.substr(backslash_pos + 1);
            shorts_count = core::safeStoiOptional(count_str, "pin_count", "@BS-S");
        } else {
            // If there is no backslash, assume the entire field is the count
            cause = "";
            shorts_count = core::safeStoiOptional(first_field, "pin_count", "@BS-S");
        }
    }
    if (fields.size() > 1) {
        node_list.clear();
        for (size_t i = 1; i < fields.size(); ++i) {
            node_list.push_back(fields[i]);
        }
    }
}

json BsShortRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_CAUSE] = cause;
    j[JSON_KEY_SHORTS_COUNT] = shorts_count.has_value() ? json(*shorts_count) : json(nullptr);
    j[JSON_KEY_NODE_LIST] = node_list;
    return j;
}

// ===================== BsOpenRecord =====================
void BsOpenRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) first_device_name = fields[0];
    if (fields.size() > 1) first_device_pin = core::safeStoi(fields[1], "first_device_pin", "@BS-O");
    if (fields.size() > 2) second_device_name = fields[2];
    else second_device_name = "";
    if (fields.size() > 3) second_device_pin = core::safeStoiOptional(fields[3], "second_device_pin", "@BS-O");
}
json BsOpenRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_FIRST_DEVICE_NAME]  = first_device_name;
    j[JSON_KEY_FIRST_DEVICE_PIN]   = first_device_pin;
    j[JSON_KEY_SECOND_DEVICE_NAME] = second_device_name == "" ? json(second_device_name) : json(nullptr);;
    j[JSON_KEY_SECOND_DEVICE_PIN]  = second_device_pin.has_value() ? json(*second_device_pin) : json(nullptr);
    return j;
}

// ===================== BoundaryScanRecord =====================
void BoundaryScanRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) test_designator = fields[0];
    if (fields.size() > 1) status          = core::safeStoi(fields[1], "status", "@CON");
    if (fields.size() > 2) shorts_count     = core::safeStoiOptional(fields[2], "shorts_count", "@BS-CON");
    if (fields.size() > 3) opens_count     = core::safeStoiOptional(fields[3], "opens_count", "@BS-CON");
}
json BoundaryScanRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_TEST_DESIGNATOR] = test_designator;
    j[JSON_KEY_STATUS]          = status;
    j[JSON_KEY_SHORTS_COUNT]    = shorts_count.has_value() ? json(*shorts_count) : json(nullptr);
    j[JSON_KEY_OPENS_COUNT]     = opens_count.has_value() ? json(*opens_count) : json(nullptr);
    return j;
}

// ===================== BTestRecord =====================
void BTestRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) board_id          = fields[0];
    if (fields.size() > 1) test_status       = core::safeStoi(fields[1], "test_status", "@BTEST");
    if (fields.size() > 2) start_datetime    = core::safeStoull(fields[2], "start_datetime", "@BTEST");
    if (fields.size() > 3) duration          = core::safeStoi(fields[3], "duration", "@BTEST");
    if (fields.size() > 4) multiple_test     = core::safeStob(fields[4], "multiple_test", "@BTEST");
    if (fields.size() > 5) log_level         = fields[5];
    if (fields.size() > 6) log_set           = core::safeStoiOptional(fields[6], "log_set", "@BTEST");
    if (fields.size() > 7) learning          = core::safeStob(fields[7], "learning", "@BTEST");
    if (fields.size() > 8) known_good        = core::safeStob(fields[8], "known_good", "@BTEST");
    if (fields.size() > 9) end_datetime      = core::safeStoull(fields[9], "end_datetime", "@BTEST");
    if (fields.size() > 10) status_qualifier = fields[10];
    if (fields.size() > 11) board_number     = core::safeStoi(fields[11], "board_number", "@BTEST");
    if (fields.size() > 12) parent_panel_id  = fields[12];
    parent_panel_id.erase(remove(parent_panel_id.begin(), parent_panel_id.end(), '\r'), parent_panel_id.end());
    parent_panel_id.erase(remove(parent_panel_id.begin(), parent_panel_id.end(), '\n'), parent_panel_id.end());
}
json BTestRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_BOARD_ID]         = board_id;
    j[JSON_KEY_TEST_STATUS]      = test_status;
    j[JSON_KEY_START_DATETIME]   = start_datetime;
    j[JSON_KEY_DURATION]         = duration;
    j[JSON_KEY_MULTIPLE_TEST]    = multiple_test;
    j[JSON_KEY_LOG_LEVEL]        = log_level;
    j[JSON_KEY_LOG_SET]          = log_set.has_value() ? json(*log_set) : json(nullptr);
    j[JSON_KEY_LEARNING]         = learning;
    j[JSON_KEY_KNOWN_GOOD]       = known_good;
    j[JSON_KEY_END_DATETIME]     = end_datetime;
    j[JSON_KEY_STATUS_QUALIFIER] = status_qualifier;
    j[JSON_KEY_BOARD_NUMBER]     = board_number;
    j[JSON_KEY_PARENT_PANEL_ID]  = parent_panel_id;
    return j;
}

} // namespace records
} // namespace keysight_log
