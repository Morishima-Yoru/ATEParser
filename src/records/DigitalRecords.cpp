/**
 * @file DigitalRecords.cpp
 * @brief Implements parsing and JSON serialization for digital log records.
 *
 * This source provides:
 *  - splitFields(): handles normal, list ('\'), and literal ('~') fields
 *  - getSubstatusFlags() and setSubstatusFlags() for DigitalTestRecord
 *  - toJson() functions for each record to build structured JSON
 *
 * Based strictly on Keysight i3070 ICT Log Record Format (Tables 8-29–8-34, Sections "How Log Records are Formatted") [^1].
 */

#include "i3070/records/DigitalRecords.hpp"
#include "i3070/utils/SafeConversion.hpp"
#include "i3070/utils/JsonKeys.hpp"
#include "i3070/core/FieldValue.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace records {

using core::FieldValue;

// ===================== DigitalTestRecord =====================

void DigitalTestRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) test_status = static_cast<enums::DigitalTestStatus>(core::safeStoi(fields[0], "test_status", "@D-T", 0));
    if (fields.size() > 1) test_substatus = core::safeStoi(fields[1], "test_substatus", "@D-T", 0);
    if (fields.size() > 2) {failing_vector_number = core::safeStoiOptional(fields[2], "failing_vector_number", "@D-T");}
    if (fields.size() > 3) pin_count = core::safeStoi(fields[3], "pin_count", "@D-T", 0);
    if (fields.size() > 4) test_designator = fields[4];
    test_designator.erase(remove(test_designator.begin(), test_designator.end(), '\r'), test_designator.end());
    test_designator.erase(remove(test_designator.begin(), test_designator.end(), '\n'), test_designator.end());
}

json DigitalTestRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_TEST_STATUS] = static_cast<int>(test_status);
    j[JSON_KEY_SUBSTATUS] = test_substatus;
    j[JSON_KEY_FAILING_VECTOR] = failing_vector_number.has_value() ? json(*failing_vector_number) : json(nullptr);
    j[JSON_KEY_PIN_COUNT] = pin_count;
    j[JSON_KEY_TEST_DESIGNATOR] = test_designator;
    return j;
}

vector<enums::DigitalTestSubstatus> DigitalTestRecord::getSubstatusFlags() const {
    vector<enums::DigitalTestSubstatus> flags;
    for (int bit = 0; bit < 6; ++bit) {
        int mask = 1 << bit;
        if (test_substatus & mask) {
            flags.push_back(static_cast<enums::DigitalTestSubstatus>(mask));
        }
    }
    return flags;
}

void DigitalTestRecord::setSubstatusFlags(const vector<enums::DigitalTestSubstatus>& flags) {
    test_substatus = 0;
    for (auto f : flags) {
        test_substatus |= static_cast<int>(f);
    }
}

// ===================== DevicePinRecord =====================

void DevicePinRecord::fromFields(const vector<string>& fields) {
    // Clear previous data
    device_name.clear();
    node_pin_list.clear();

    // device_name is the part before the first '\' in fields[1]
    if (fields.size() > 0) {
        string::size_type pos = fields[0].find('\\');
        if (pos != string::npos) {
            device_name = fields[0].substr(0, pos);
        } else {
            device_name = fields[0];
        }
    }

    // Parse node_pin_list according to the custom schema
    // 1. fields[2] as string
    // 2. fields[3] as string (before '\'), if contains '\', use after '\' as N, group next N fields as array
    size_t i = 1;
    if (fields.size() > i) {
        node_pin_list.push_back(fields[i]); // fields[2] as string
        ++i;
    }
    if (fields.size() > i) {
        string field = fields[i];
        string::size_type pos = field.find('\\');
        if (pos != string::npos) {
            // Take before '\' as string
            string base = field.substr(0, pos);
            node_pin_list.push_back(base);
            // Take after '\' as N
            int arr_len = std::stoi(field.substr(pos + 1));
            std::vector<std::string> arr;
            for (int j = 0; j < arr_len && (i + 1 + j) < fields.size(); ++j) {
                arr.push_back(fields[i + 1 + j]);
            }
            node_pin_list.push_back(arr);
            i += 1 + arr_len;
        } else {
            node_pin_list.push_back(field);
            ++i;
        }
    }
    // If there are any remaining fields, push as string (fallback, for robustness)
    while (i < fields.size()) {
        node_pin_list.push_back(fields[i]);
        ++i;
    }
}

void DevicePinRecord::addNodePin(const string& node_id, const string& device_pin) {
    node_pin_list.push_back(node_id);
    node_pin_list.push_back(device_pin);
}

void DevicePinRecord::addDriveThruPair(const string& node_id, const string& device_id) {
    if (!thru_devnode_list.has_value()) {
        thru_devnode_list.emplace();
    }
    thru_devnode_list->push_back(node_id);
    thru_devnode_list->push_back(device_id);
}

json DevicePinRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_DEVICE_NAME] = device_name;
    j[JSON_KEY_NODE_PIN_LIST] = node_pin_list;
    return j;
}

// ===================== PLDProgrammingRecord =====================

void PLDProgrammingRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) filename = fields[0];
    if (fields.size() > 1) action = fields[1];
    if (fields.size() > 2) action_return_code = core::safeStoi(fields[2], "action_return_code", "@D-PLD", 0);
    if (fields.size() > 3) result_message = fields[3];
    if (fields.size() > 4) player_program_counter = core::safeStoi(fields[4], "player_program_counter", "@D-PLD", 0);
}

json PLDProgrammingRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_FILENAME] = filename;
    j[JSON_KEY_ACTION] = action;
    j[JSON_KEY_ACTION_RETURN_CODE] = action_return_code;
    j[JSON_KEY_RESULT_MESSAGE] = result_message;
    j[JSON_KEY_PLAYER_PROGRAM_COUNTER] = player_program_counter;
    return j;
}

json toJson(const PLDProgrammingRecord& rec) {
    return json{
        {JSON_KEY_PREFIX, prefixToString(rec.prefix)},
        {JSON_KEY_FILENAME, rec.filename},
        {JSON_KEY_ACTION, rec.action},
        {JSON_KEY_ACTION_RETURN_CODE, rec.action_return_code},
        {JSON_KEY_RESULT_MESSAGE, rec.result_message},
        {JSON_KEY_PLAYER_PROGRAM_COUNTER, rec.player_program_counter}
    };
}

// ===================== ExportRecord =====================

void ExportRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) key = fields[0];
    if (fields.size() > 1) field = fields[1];
    field.erase(remove(field.begin(), field.end(), '\r'), field.end());
    field.erase(remove(field.begin(), field.end(), '\n'), field.end());
    
}

json ExportRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_KEY] = key;
    j[JSON_KEY_FIELD] = field;
    return j;
}

json toJson(const ExportRecord& rec) {
    return json{
        {JSON_KEY_PREFIX, prefixToString(rec.prefix)},
        {JSON_KEY_KEY, rec.key},
        {JSON_KEY_FIELD, rec.field}
    };
}

// ===================== NoteRecord =====================

void NoteRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) note_name = fields[0];
    if (fields.size() > 1) note_string = fields[1];
    note_string.erase(remove(note_string.begin(), note_string.end(), '\r'), note_string.end());
    note_string.erase(remove(note_string.begin(), note_string.end(), '\n'), note_string.end());
}

json NoteRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_NOTE_NAME] = note_name;
    j[JSON_KEY_NOTE_STRING] = note_string;
    return j;
}

json toJson(const NoteRecord& rec) {
    return json{
        {JSON_KEY_PREFIX, prefixToString(rec.prefix)},
        {JSON_KEY_NOTE_NAME, rec.note_name},
        {JSON_KEY_NOTE_STRING, rec.note_string}
    };
}

// ===================== ConnectCheckRecord =====================

void ConnectCheckRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) test_status = static_cast<enums::GenericTestStatus>(core::safeStoi(fields[0], "test_status", "@CCHK", 0));
    if (fields.size() > 1) pin_count = core::safeStoi(fields[1], "pin_count", "@CCHK", 0);
    if (fields.size() > 2) device_designator = fields[2];
    device_designator.erase(remove(device_designator.begin(), device_designator.end(), '\r'), device_designator.end());
    device_designator.erase(remove(device_designator.begin(), device_designator.end(), '\n'), device_designator.end());
}

json ConnectCheckRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_STATUS] = static_cast<int>(test_status);
    j[JSON_KEY_PIN_COUNT] = pin_count;
    j[JSON_KEY_DEVICE_DESIGNATOR] = device_designator;
    return j;
}

json toJson(const ConnectCheckRecord& rec) {
    return json{
        {JSON_KEY_PREFIX, prefixToString(rec.prefix)},
        {JSON_KEY_STATUS, static_cast<int>(rec.test_status)},
        {JSON_KEY_PIN_COUNT, rec.pin_count},
        {JSON_KEY_DEVICE_DESIGNATOR, rec.device_designator}
    };
}

// ===================== PolarityCheckRecord =====================

void PolarityCheckRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) test_status = static_cast<enums::GenericTestStatus>(core::safeStoi(fields[0], "test_status", "@POL", 0));
    if (fields.size() > 1) test_designator = fields[1];
    test_designator.erase(remove(test_designator.begin(), test_designator.end(), '\r'), test_designator.end());
    test_designator.erase(remove(test_designator.begin(), test_designator.end(), '\n'), test_designator.end());
}

json PolarityCheckRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_STATUS] = static_cast<int>(test_status);
    j[JSON_KEY_DESIGNATOR] = test_designator;
    return j;
}

json toJson(const PolarityCheckRecord& rec) {
    return json{
        {JSON_KEY_PREFIX, prefixToString(rec.prefix)},
        {JSON_KEY_STATUS, static_cast<int>(rec.test_status)},
        {JSON_KEY_DESIGNATOR, rec.test_designator}
    };
}

// ===================== TestJetRecord =====================

void TestJetRecord::fromFields(const vector<string>& fields) {
    // Format: {@TJET|test status|pin count|test designator}
    if (fields.size() > 0) test_status = static_cast<enums::GenericTestStatus>(core::safeStoi(fields[0], "test_status", "@TJET", 0));
    if (fields.size() > 1) pin_count = core::safeStoi(fields[1], "pin_count", "@TJET", 0);
    if (fields.size() > 2) test_designator = fields[2];
    test_designator.erase(remove(test_designator.begin(), test_designator.end(), '\r'), test_designator.end());
    test_designator.erase(remove(test_designator.begin(), test_designator.end(), '\n'), test_designator.end());
}

json TestJetRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_TEST_STATUS] = static_cast<int>(test_status);
    j[JSON_KEY_PIN_COUNT] = pin_count;
    j[JSON_KEY_TEST_DESIGNATOR] = test_designator;
    return j;
}

json toJson(const TestJetRecord& rec) {
    return json{
        {JSON_KEY_PREFIX, prefixToString(rec.prefix)},
        {JSON_KEY_STATUS, static_cast<int>(rec.test_status)},
        {JSON_KEY_PIN_COUNT, rec.pin_count},
        {JSON_KEY_DESIGNATOR, rec.test_designator}
    };
}

// ===================== IndictmentRecord =====================

void IndictmentRecord::fromFields(const vector<string>& fields) {
    device_list.clear();
    technique.clear();

    int device_count = 0;
    if (fields.size() > 0) {
        std::string::size_type pos = fields[0].find('\\');
        if (pos != std::string::npos) {
            technique = fields[0].substr(0, pos);
            std::string count_str = fields[0].substr(pos + 1);
            device_count = core::safeStoi(count_str, "device_count", "@INDICT", 0);
        } else {
            technique = fields[0];
        }
    }

    for (int i = 0; i < device_count && (1 + i) < fields.size(); ++i) {
        device_list.push_back(fields[1 + i]);
    }

    size_t next_field = 1 + device_count;
    if (fields.size() > next_field)     est_resistance  = core::safeStodOptional(fields[next_field], "est_resistance", "@INDICT");
    if (fields.size() > next_field + 1) est_capacitance = core::safeStodOptional(fields[next_field + 1], "est_capacitance", "@INDICT");
    if (fields.size() > next_field + 2) est_inductance  = core::safeStodOptional(fields[next_field + 2], "est_inductance", "@INDICT");
    if (fields.size() > next_field + 3) {
        est_model = fields[next_field + 3];
        est_model.erase(remove(est_model.begin(), est_model.end(), '\r'), est_model.end());
        est_model.erase(remove(est_model.begin(), est_model.end(), '\n'), est_model.end());
    }
}

json IndictmentRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_TECHNIQUE] = technique;
    j[JSON_KEY_DEVICE_LIST] = device_list;
    j[JSON_KEY_EST_RESISTANCE] = est_resistance.has_value() ? json(*est_resistance) : json(nullptr);
    j[JSON_KEY_EST_CAPACITANCE] = est_capacitance.has_value() ? json(*est_capacitance) : json(nullptr);
    j[JSON_KEY_EST_INDUCTANCE] = est_inductance.has_value() ? json(*est_inductance) : json(nullptr);
    j[JSON_KEY_EST_MODEL] = est_model;
    return j;
}

json toJson(const IndictmentRecord& rec) {
    json j;
    j[JSON_KEY_PREFIX]      = prefixToString(rec.prefix);
    j[JSON_KEY_TECHNIQUE]   = rec.technique;
    j[JSON_KEY_DEVICE_LIST] = rec.device_list;
    return j;
}

// ===================== PinRecord =====================

void PinRecord::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) {
        // The format of the first field: "\\count"
        string first_field = fields[0];
        size_t backslash_pos = first_field.find('\\');
        if (backslash_pos != string::npos) {
            string count_str = first_field.substr(backslash_pos + 1);
            pin_count = core::safeStoiOptional(count_str, "pin_count", "@PIN");
        } else {
            // If there is no backslash, assume the entire field is the count
            pin_count = core::safeStoiOptional(first_field, "pin_count", "@PIN");
        }
    }
    if (fields.size() > 1) {
        pins.clear();
        for (size_t i = 1; i < fields.size(); ++i) {
            pins.push_back(fields[i]);
        }
    }
}

json PinRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_PIN_COUNT] = pin_count.has_value() ? json(*pin_count) : json(nullptr);
    j[JSON_KEY_PINS] = pins;
    return j;
}

} // namespace records
} // namespace i3070
