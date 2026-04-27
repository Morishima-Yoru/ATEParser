/**
 * @file record.cpp
 * @brief Implementation of make_record / parse_into / to_json / prefix_of.
 *
 * Each concrete record type owns:
 *   - a `parse_record(rec, fields)` free function (in this TU), and
 *   - a `to_json_record(rec)` free function (in this TU).
 *
 * The variant-level entry points dispatch via std::visit + the
 * `overloaded` lambda set defined in record.hpp.
 */
#include "ate_parser/core/record.hpp"
#include "ate_parser/utils/errors.hpp"
#include "ate_parser/utils/json_keys.hpp"
#include "ate_parser/utils/logging.hpp"
#include "ate_parser/utils/safe_conversion.hpp"

#include <algorithm>
#include <string>
#include <string_view>

using nlohmann::json;
using ate::utils::parse_or_default;
using ate::utils::parse_or_throw;
using ate::utils::parse_to;

namespace ate::core {
namespace {

namespace jk = ate::json_keys;

// ---- small string helpers ------------------------------------------------

inline std::string strip_eol(std::string s) {
    s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
    s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
    return s;
}

template <typename T>
json opt_to_json(const std::optional<T>& o) {
    return o.has_value() ? json(*o) : json(nullptr);
}

// =====================================================================
//  parse_record overloads (one per concrete record type)
// =====================================================================

void parse_record(std::monostate&, const std::vector<std::string>&) { /* no-op */ }

// ---- Limits ---------------------------------------------------------------
void parse_record(records::Lim2Record& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.high_limit = parse_or_default<double>(f[0], "high_limit", "@LIM2", 0.0);
    if (f.size() > 1) r.low_limit  = parse_or_default<double>(f[1], "low_limit",  "@LIM2", 0.0);
}
void parse_record(records::Lim3Record& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.nominal_value = parse_or_default<double>(f[0], "nominal_value", "@LIM3", 0.0);
    if (f.size() > 1) r.high_limit    = parse_or_default<double>(f[1], "high_limit",    "@LIM3", 0.0);
    if (f.size() > 2) r.low_limit     = parse_or_default<double>(f[2], "low_limit",     "@LIM3", 0.0);
}

// ---- Analog test ----------------------------------------------------------
void parse_record(records::AnalogTestRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) {
        const int code = parse_or_default<int>(f[0], "test_status", "@ANALOG", 0);
        try { r.test_status = enums::to_analog_status(code); }
        catch (const std::out_of_range&) {
            r.test_status = enums::AnalogTestStatus::failed_general;
            ATE_LOG_WARN("@ANALOG: unknown test_status code {}", code);
        }
    }
    if (f.size() > 1) {
        auto v = parse_to<double>(f[1]);
        if (v) r.measured_value = *v;
    }
    if (f.size() > 2 && !f[2].empty()) {
        r.subtest_designator = strip_eol(f[2]);
        if (r.subtest_designator->empty()) r.subtest_designator.reset();
    }
}

// ---- Digital --------------------------------------------------------------
void parse_record(records::DigitalTestRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) {
        const int code = parse_or_default<int>(f[0], "test_status", "@D-T", 0);
        try { r.test_status = enums::to_digital_status(code); }
        catch (const std::out_of_range&) { r.test_status = enums::DigitalTestStatus::failed; }
    }
    if (f.size() > 1) r.test_substatus        = parse_or_default<int>(f[1], "test_substatus", "@D-T", 0);
    if (f.size() > 2) {
        auto v = parse_to<int>(f[2]);
        r.failing_vector_number = v ? std::optional<int>(*v) : std::nullopt;
    }
    if (f.size() > 3) r.pin_count             = parse_or_default<int>(f[3], "pin_count",      "@D-T", 0);
    if (f.size() > 4) r.test_designator       = strip_eol(f[4]);
}

void parse_record(records::DevicePinRecord& r, const std::vector<std::string>& f) {
    r.device_name.clear();
    r.node_pin_list = json::array();

    if (!f.empty()) {
        auto pos = f[0].find('\\');
        r.device_name = (pos == std::string::npos) ? f[0] : f[0].substr(0, pos);
    }

    std::size_t i = 1;
    if (f.size() > i) {
        r.node_pin_list.push_back(f[i]);
        ++i;
    }
    if (f.size() > i) {
        const std::string& field = f[i];
        auto pos = field.find('\\');
        if (pos != std::string::npos) {
            const std::string base = field.substr(0, pos);
            r.node_pin_list.push_back(base);
            const int arr_len = parse_or_default<int>(field.substr(pos + 1), "group_size", "@DPIN", 0);
            json arr = json::array();
            for (int j = 0; j < arr_len && (i + 1 + static_cast<std::size_t>(j)) < f.size(); ++j) {
                arr.push_back(f[i + 1 + j]);
            }
            r.node_pin_list.push_back(arr);
            i += 1 + static_cast<std::size_t>(arr_len);
        } else {
            r.node_pin_list.push_back(field);
            ++i;
        }
    }
    while (i < f.size()) {
        r.node_pin_list.push_back(f[i]);
        ++i;
    }
}

void parse_record(records::PldProgrammingRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.filename               = f[0];
    if (f.size() > 1) r.action                 = f[1];
    if (f.size() > 2) r.action_return_code     = parse_or_default<int>(f[2], "action_return_code",     "@D-PLD", 0);
    if (f.size() > 3) r.result_message         = f[3];
    if (f.size() > 4) r.player_program_counter = parse_or_default<int>(f[4], "player_program_counter", "@D-PLD", 0);
}

void parse_record(records::ConnectCheckRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(parse_or_default<int>(f[0], "test_status", "@CCHK", 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.pin_count         = parse_or_default<int>(f[1], "pin_count", "@CCHK", 0);
    if (f.size() > 2) r.device_designator = strip_eol(f[2]);
}

void parse_record(records::PolarityCheckRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(parse_or_default<int>(f[0], "test_status", "@PCHK", 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.test_designator = strip_eol(f[1]);
}

void parse_record(records::TestJetRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(parse_or_default<int>(f[0], "test_status", "@TJET", 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.pin_count       = parse_or_default<int>(f[1], "pin_count", "@TJET", 0);
    if (f.size() > 2) r.test_designator = strip_eol(f[2]);
}

void parse_record(records::IndictmentRecord& r, const std::vector<std::string>& f) {
    r.device_list.clear();
    r.technique.clear();

    int device_count = 0;
    if (!f.empty()) {
        auto pos = f[0].find('\\');
        if (pos != std::string::npos) {
            r.technique  = f[0].substr(0, pos);
            device_count = parse_or_default<int>(f[0].substr(pos + 1), "device_count", "@INDICT", 0);
        } else {
            r.technique = f[0];
        }
    }
    for (int i = 0; i < device_count && (1 + static_cast<std::size_t>(i)) < f.size(); ++i) {
        r.device_list.push_back(f[1 + i]);
    }
    const std::size_t next = 1 + static_cast<std::size_t>(device_count);
    if (f.size() > next)     { auto v = parse_to<double>(f[next]);     r.est_resistance  = v ? std::optional<double>(*v) : std::nullopt; }
    if (f.size() > next + 1) { auto v = parse_to<double>(f[next + 1]); r.est_capacitance = v ? std::optional<double>(*v) : std::nullopt; }
    if (f.size() > next + 2) { auto v = parse_to<double>(f[next + 2]); r.est_inductance  = v ? std::optional<double>(*v) : std::nullopt; }
    if (f.size() > next + 3) r.est_model = strip_eol(f[next + 3]);
}

void parse_record(records::PinRecord& r, const std::vector<std::string>& f) {
    r.pins.clear();
    if (!f.empty()) {
        const std::string& first = f[0];
        auto pos = first.find('\\');
        const std::string count_str = (pos == std::string::npos) ? first : first.substr(pos + 1);
        auto v = parse_to<int>(count_str);
        r.pin_count = v ? std::optional<int>(*v) : std::nullopt;
    }
    for (std::size_t i = 1; i < f.size(); ++i) r.pins.push_back(f[i]);
}

// ---- Shorts ---------------------------------------------------------------
void parse_record(records::ShortsTestRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(parse_or_default<int>(f[0], "test_status", "@TS", 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.shorts_count   = parse_or_default<int>(f[1], "shorts_count",   "@TS", 0);
    if (f.size() > 2) r.opens_count    = parse_or_default<int>(f[2], "opens_count",    "@TS", 0);
    if (f.size() > 3) r.phantoms_count = parse_or_default<int>(f[3], "phantoms_count", "@TS", 0);
    if (f.size() > 4) r.designator     = strip_eol(f[4]);
}

void parse_record(records::TsSourceRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.shorts_count   = parse_or_default<int>(f[0], "shorts_count",   "@TS-S", 0);
    if (f.size() > 1) r.phantoms_count = parse_or_default<int>(f[1], "phantoms_count", "@TS-S", 0);
    if (f.size() > 2) r.source_node    = strip_eol(f[2]);
}

void parse_record(records::TsDestinationRecord& r, const std::vector<std::string>& f) {
    r.destination_list.clear();
    // Field 0 is the "\count" prefix; pairs start at index 1.
    for (std::size_t i = 1; i + 1 < f.size(); i += 2) {
        const double dev = parse_or_default<double>(f[i + 1], "deviation", "@TS-D", 0.0);
        r.destination_list.emplace_back(f[i], dev);
    }
}

void parse_record(records::TsOpenRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.source_node      = f[0];
    if (f.size() > 1) r.destination_node = f[1];
    if (f.size() > 2) { auto v = parse_to<double>(f[2]); r.deviation = v ? std::optional<double>(*v) : std::nullopt; }
}

void parse_record(records::TsPhantomRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) { auto v = parse_to<double>(f[0]); r.deviation = v ? std::optional<double>(*v) : std::nullopt; }
}

// ---- System ---------------------------------------------------------------
void parse_record(records::BatchRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0)  r.uut_type              = f[0];
    if (f.size() > 1)  r.uut_type_rev          = f[1];
    if (f.size() > 2)  r.fixture_id            = parse_or_default<int>(f[2], "fixture_id",      "@BATCH", 0);
    if (f.size() > 3)  r.testhead_number       = parse_or_default<int>(f[3], "testhead_number", "@BATCH", 1);
    if (f.size() > 4)  r.testhead_type         = f[4];
    if (f.size() > 5)  r.process_step          = f[5];
    if (f.size() > 6)  r.batch_id              = f[6];
    if (f.size() > 7)  r.operator_id           = f[7];
    if (f.size() > 8)  r.controller            = f[8];
    if (f.size() > 9)  r.testplan_id           = f[9];
    if (f.size() > 10) r.testplan_rev          = f[10];
    if (f.size() > 11) r.parent_panel_type     = f[11];
    if (f.size() > 12) r.parent_panel_type_rev = f[12];
    if (f.size() > 13) r.version_label         = strip_eol(f[13]);
}

void parse_record(records::BlockRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.block_designator = f[0];
    if (f.size() > 1) r.block_status     = parse_or_default<int>(f[1], "block_status", "@BLOCK", 0);
}

void parse_record(records::AlarmRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.alarm_type      = parse_or_default<int>(f[0], "alarm_type",   "@ALM", 1);
    if (f.size() > 1) r.alarm_status    = (parse_or_default<int>(f[1], "alarm_status","@ALM", 0) != 0);
    if (f.size() > 2) r.datetime        = f[2];
    if (f.size() > 3) r.board_type      = f[3];
    if (f.size() > 4) r.board_rev       = f[4];
    if (f.size() > 5) { auto v = parse_to<int>(f[5]); r.alarm_limit    = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 6) { auto v = parse_to<int>(f[6]); r.detected_value = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 7) r.controller      = f[7];
    if (f.size() > 8) r.testhead_number = parse_or_default<int>(f[8], "testhead_number", "@ALM", 1);
}

void parse_record(records::AlarmBoardRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.datetime = f[0];
    if (f.size() > 1) r.serial   = strip_eol(f[1]);
}

void parse_record(records::ArrayRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.designator = f[0];
    if (f.size() > 1) r.status     = parse_or_default<int>(f[1], "status", "@ARRAY", 0);
    if (f.size() > 2) { auto v = parse_to<int>(f[2]); r.failure_count = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 3) { auto v = parse_to<int>(f[3]); r.samples       = v ? std::optional<int>(*v) : std::nullopt; }
}

void parse_record(records::NetVerifyRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.datetime      = f[0];
    if (f.size() > 1) r.test_system   = f[1];
    if (f.size() > 2) r.repair_system = f[2];
    if (f.size() > 3) r.source        = (parse_or_default<int>(f[3], "source", "@NETV", 0) != 0);
}

void parse_record(records::NodeListRecord& r, const std::vector<std::string>& f) {
    r.nodes.clear();
    if (f.empty()) return;
    auto pos = f[0].find('\\');
    if (pos != std::string::npos) {
        r.count = parse_or_default<int>(f[0].substr(pos + 1), "count", "@NODE", 0);
    } else {
        r.count = 1;
        r.nodes.push_back(f[0]);
        return;
    }
    for (std::size_t i = 1; i < f.size(); ++i) r.nodes.push_back(f[i]);
}

void parse_record(records::ReportRecord& r, const std::vector<std::string>& f) {
    r.message.clear();
    for (std::size_t i = 0; i < f.size(); ++i) {
        if (i > 0) r.message.push_back('|');
        r.message += f[i];
    }
}

void parse_record(records::RetestRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.datetime = strip_eol(f[0]);
}

void parse_record(records::PfRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.designator  = f[0];
    if (f.size() > 1) { auto v = parse_to<int>(f[1]); r.test_status = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 2) { auto v = parse_to<int>(f[2]); r.total_pins  = v ? std::optional<int>(*v) : std::nullopt; }
}

void parse_record(records::BsShortRecord& r, const std::vector<std::string>& f) {
    r.node_list.clear();
    if (f.empty()) return;
    const std::string& first = f[0];
    auto pos = first.find('\\');
    if (pos != std::string::npos) {
        r.cause = first.substr(0, pos);
        auto v  = parse_to<int>(first.substr(pos + 1));
        r.shorts_count = v ? std::optional<int>(*v) : std::nullopt;
    } else {
        r.cause.clear();
        auto v = parse_to<int>(first);
        r.shorts_count = v ? std::optional<int>(*v) : std::nullopt;
    }
    for (std::size_t i = 1; i < f.size(); ++i) r.node_list.push_back(f[i]);
}

void parse_record(records::BsOpenRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.first_device_name  = f[0];
    if (f.size() > 1) r.first_device_pin   = f[1];
    if (f.size() > 2) r.second_device_name = f[2];
    if (f.size() > 3) r.second_device_pin  = f[3];
}

void parse_record(records::BoundaryScanRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0) r.test_designator = f[0];
    if (f.size() > 1) r.status          = parse_or_default<int>(f[1], "status", "@BS-CON", 0);
    if (f.size() > 2) { auto v = parse_to<int>(f[2]); r.shorts_count = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 3) { auto v = parse_to<int>(f[3]); r.opens_count  = v ? std::optional<int>(*v) : std::nullopt; }
}

void parse_record(records::BTestRecord& r, const std::vector<std::string>& f) {
    if (f.size() > 0)  r.board_id         = f[0];
    if (f.size() > 1)  r.test_status      = parse_or_default<int>(f[1], "test_status", "@BTEST", 0);
    if (f.size() > 2)  r.start_datetime   = parse_or_default<unsigned long long>(f[2], "start_datetime", "@BTEST", 0ull);
    if (f.size() > 3)  r.duration         = parse_or_default<int>(f[3], "duration", "@BTEST", 0);
    if (f.size() > 4)  r.multiple_test    = utils::parse_bool_or_default(f[4], "multiple_test", "@BTEST", false);
    if (f.size() > 5)  r.log_level        = f[5];
    if (f.size() > 6)  { auto v = parse_to<int>(f[6]); r.log_set = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 7)  r.learning         = utils::parse_bool_or_default(f[7], "learning",   "@BTEST", false);
    if (f.size() > 8)  r.known_good       = utils::parse_bool_or_default(f[8], "known_good", "@BTEST", false);
    if (f.size() > 9)  r.end_datetime     = parse_or_default<unsigned long long>(f[9], "end_datetime", "@BTEST", 0ull);
    if (f.size() > 10) r.status_qualifier = f[10];
    if (f.size() > 11) r.board_number     = parse_or_default<int>(f[11], "board_number", "@BTEST", 0);
    if (f.size() > 12) r.parent_panel_id  = strip_eol(f[12]);
}

void parse_record(records::ProbeRecord& r, const std::vector<std::string>& f) {
    r.raw_payload.clear();
    for (std::size_t i = 0; i < f.size(); ++i) {
        if (i > 0) r.raw_payload.push_back('|');
        r.raw_payload += f[i];
    }
}

// =====================================================================
//  to_json_record overloads
// =====================================================================

template <typename Rec>
json base_json(const Rec& r) {
    return json{ {std::string{jk::prefix}, enums::to_string(r.prefix)} };
}

json to_json_record(std::monostate)              { return json::object(); }

json to_json_record(const records::Lim2Record& r) {
    json j = base_json(r);
    j[std::string{jk::high_limit}] = r.high_limit;
    j[std::string{jk::low_limit}]  = r.low_limit;
    return j;
}
json to_json_record(const records::Lim3Record& r) {
    json j = base_json(r);
    j[std::string{jk::nominal_value}] = r.nominal_value;
    j[std::string{jk::high_limit}]    = r.high_limit;
    j[std::string{jk::low_limit}]     = r.low_limit;
    return j;
}
json to_json_record(const records::AnalogTestRecord& r) {
    json j {
        {std::string{jk::prefix},      enums::to_string(r.prefix)},
        {std::string{jk::test_status}, static_cast<int>(r.test_status)},
    };
    if (r.measured_value)     j[std::string{jk::measured_value}]     = *r.measured_value;
    if (r.subtest_designator && !r.subtest_designator->empty())
                              j[std::string{jk::subtest_designator}] = *r.subtest_designator;
    return j;
}

json to_json_record(const records::DigitalTestRecord& r) {
    json j = base_json(r);
    j[std::string{jk::test_status}]     = static_cast<int>(r.test_status);
    j[std::string{jk::substatus}]       = r.test_substatus;
    j[std::string{jk::failing_vector}]  = opt_to_json(r.failing_vector_number);
    j[std::string{jk::pin_count}]       = r.pin_count;
    j[std::string{jk::test_designator}] = r.test_designator;
    return j;
}
json to_json_record(const records::DevicePinRecord& r) {
    json j = base_json(r);
    j[std::string{jk::device_name}]   = r.device_name;
    j[std::string{jk::node_pin_list}] = r.node_pin_list;
    if (r.thru_devnode_list) j[std::string{jk::thru_devnode_list}] = *r.thru_devnode_list;
    return j;
}
json to_json_record(const records::PldProgrammingRecord& r) {
    json j = base_json(r);
    j[std::string{jk::filename}]               = r.filename;
    j[std::string{jk::action}]                 = r.action;
    j[std::string{jk::action_return_code}]     = r.action_return_code;
    j[std::string{jk::result_message}]         = r.result_message;
    j[std::string{jk::player_program_counter}] = r.player_program_counter;
    return j;
}
json to_json_record(const records::ConnectCheckRecord& r) {
    json j = base_json(r);
    j[std::string{jk::status}]            = static_cast<int>(r.test_status);
    j[std::string{jk::pin_count}]         = r.pin_count;
    j[std::string{jk::device_designator}] = r.device_designator;
    return j;
}
json to_json_record(const records::PolarityCheckRecord& r) {
    json j = base_json(r);
    j[std::string{jk::status}]     = static_cast<int>(r.test_status);
    j[std::string{jk::designator}] = r.test_designator;
    return j;
}
json to_json_record(const records::TestJetRecord& r) {
    json j = base_json(r);
    j[std::string{jk::status}]     = static_cast<int>(r.test_status);
    j[std::string{jk::pin_count}]  = r.pin_count;
    j[std::string{jk::designator}] = r.test_designator;
    return j;
}
json to_json_record(const records::IndictmentRecord& r) {
    json j = base_json(r);
    j[std::string{jk::technique}]       = r.technique;
    j[std::string{jk::device_list}]     = r.device_list;
    j[std::string{jk::est_resistance}]  = opt_to_json(r.est_resistance);
    j[std::string{jk::est_capacitance}] = opt_to_json(r.est_capacitance);
    j[std::string{jk::est_inductance}]  = opt_to_json(r.est_inductance);
    j[std::string{jk::est_model}]       = r.est_model;
    return j;
}
json to_json_record(const records::PinRecord& r) {
    json j = base_json(r);
    j[std::string{jk::pin_count}] = opt_to_json(r.pin_count);
    j[std::string{jk::pins}]      = r.pins;
    return j;
}

// Shorts
json to_json_record(const records::ShortsTestRecord& r) {
    json j = base_json(r);
    j[std::string{jk::test_status}]    = static_cast<int>(r.test_status);
    j[std::string{jk::shorts_count}]   = r.shorts_count;
    j[std::string{jk::opens_count}]    = r.opens_count;
    j[std::string{jk::phantoms_count}] = r.phantoms_count;
    j[std::string{jk::designator}]     = r.designator.empty() ? json(nullptr) : json(r.designator);
    return j;
}
json to_json_record(const records::TsSourceRecord& r) {
    json j = base_json(r);
    j[std::string{jk::shorts_count}]   = r.shorts_count;
    j[std::string{jk::phantoms_count}] = r.phantoms_count;
    j[std::string{jk::source_node}]    = r.source_node;
    return j;
}
json to_json_record(const records::TsDestinationRecord& r) {
    json arr = json::array();
    for (auto& p : r.destination_list) arr.push_back({p.first, p.second});
    json j = base_json(r);
    j[std::string{jk::destinations_list}] = arr;
    return j;
}
json to_json_record(const records::TsOpenRecord& r) {
    json j = base_json(r);
    j[std::string{jk::source_node}]      = r.source_node;
    j[std::string{jk::destination_node}] = r.destination_node;
    j[std::string{jk::deviation}]        = opt_to_json(r.deviation);
    return j;
}
json to_json_record(const records::TsPhantomRecord& r) {
    json j = base_json(r);
    j[std::string{jk::deviation}] = opt_to_json(r.deviation);
    return j;
}

// System
json to_json_record(const records::BatchRecord& r) {
    json j = base_json(r);
    j[std::string{jk::uut_type}]              = r.uut_type;
    j[std::string{jk::uut_type_rev}]          = r.uut_type_rev;
    j[std::string{jk::fixture_id}]            = r.fixture_id;
    j[std::string{jk::testhead_number}]       = r.testhead_number;
    j[std::string{jk::testhead_type}]         = r.testhead_type;
    j[std::string{jk::process_step}]          = r.process_step;
    j[std::string{jk::batch_id}]              = r.batch_id;
    j[std::string{jk::operator_id}]           = r.operator_id;
    j[std::string{jk::controller}]            = r.controller;
    j[std::string{jk::testplan_id}]           = r.testplan_id;
    j[std::string{jk::testplan_rev}]          = r.testplan_rev;
    j[std::string{jk::parent_panel_type}]     = r.parent_panel_type;
    j[std::string{jk::parent_panel_type_rev}] = r.parent_panel_type_rev;
    j[std::string{jk::version_label}]         = r.version_label;
    return j;
}
json to_json_record(const records::BlockRecord& r) {
    json j = base_json(r);
    j[std::string{jk::block_designator}] = r.block_designator;
    j[std::string{jk::block_status}]     = r.block_status;
    return j;
}
json to_json_record(const records::AlarmRecord& r) {
    json j = base_json(r);
    j[std::string{jk::alarm_type}]      = r.alarm_type;
    j[std::string{jk::alarm_status}]    = r.alarm_status;
    j[std::string{jk::datetime}]        = r.datetime;
    j[std::string{jk::board_type}]      = r.board_type;
    j[std::string{jk::board_rev}]       = r.board_rev;
    j[std::string{jk::alarm_limit}]     = opt_to_json(r.alarm_limit);
    j[std::string{jk::detected_value}]  = opt_to_json(r.detected_value);
    j[std::string{jk::controller}]      = r.controller;
    j[std::string{jk::testhead_number}] = r.testhead_number;
    return j;
}
json to_json_record(const records::AlarmBoardRecord& r) {
    json j = base_json(r);
    j[std::string{jk::datetime}]      = r.datetime;
    j[std::string{jk::serial_number}] = r.serial;
    return j;
}
json to_json_record(const records::ArrayRecord& r) {
    json j = base_json(r);
    j[std::string{jk::designator}]    = r.designator;
    j[std::string{jk::status}]        = r.status;
    j[std::string{jk::failure_count}] = opt_to_json(r.failure_count);
    j[std::string{jk::samples}]       = opt_to_json(r.samples);
    return j;
}
json to_json_record(const records::NetVerifyRecord& r) {
    json j = base_json(r);
    j[std::string{jk::datetime}]      = r.datetime;
    j[std::string{jk::test_system}]   = r.test_system;
    j[std::string{jk::repair_system}] = r.repair_system;
    j[std::string{jk::source}]        = r.source;
    return j;
}
json to_json_record(const records::NodeListRecord& r) {
    json j = base_json(r);
    j[std::string{jk::count}] = r.count;
    j[std::string{jk::nodes}] = r.nodes;
    return j;
}
json to_json_record(const records::ReportRecord& r) {
    json j = base_json(r);
    j[std::string{jk::message}] = r.message;
    return j;
}
json to_json_record(const records::RetestRecord& r) {
    json j = base_json(r);
    j[std::string{jk::datetime}] = r.datetime;
    return j;
}
json to_json_record(const records::PfRecord& r) {
    json j = base_json(r);
    j[std::string{jk::designator}]  = r.designator;
    j[std::string{jk::test_status}] = opt_to_json(r.test_status);
    j[std::string{jk::total_pins}]  = opt_to_json(r.total_pins);
    return j;
}
json to_json_record(const records::BsShortRecord& r) {
    json j = base_json(r);
    j[std::string{jk::cause}]        = r.cause;
    j[std::string{jk::shorts_count}] = opt_to_json(r.shorts_count);
    j[std::string{jk::node_list}]    = r.node_list;
    return j;
}
json to_json_record(const records::BsOpenRecord& r) {
    json j = base_json(r);
    j[std::string{jk::first_device_name}]   = r.first_device_name;
    j[std::string{jk::first_device_pin}]    = r.first_device_pin;
    j[std::string{jk::second_device_name}]  = r.second_device_name.empty() ? json(nullptr) : json(r.second_device_name);
    j[std::string{jk::second_device_pin}]   = r.second_device_pin.empty()  ? json(nullptr) : json(r.second_device_pin);
    return j;
}
json to_json_record(const records::BoundaryScanRecord& r) {
    json j = base_json(r);
    j[std::string{jk::test_designator}] = r.test_designator;
    j[std::string{jk::status}]          = r.status;
    j[std::string{jk::shorts_count}]    = opt_to_json(r.shorts_count);
    j[std::string{jk::opens_count}]     = opt_to_json(r.opens_count);
    return j;
}
json to_json_record(const records::BTestRecord& r) {
    json j = base_json(r);
    j[std::string{jk::board_id}]         = r.board_id;
    j[std::string{jk::test_status}]      = r.test_status;
    j[std::string{jk::start_datetime}]   = r.start_datetime;
    j[std::string{jk::duration}]         = r.duration;
    j[std::string{jk::multiple_test}]    = r.multiple_test;
    j[std::string{jk::log_level}]        = r.log_level;
    j[std::string{jk::log_set}]          = opt_to_json(r.log_set);
    j[std::string{jk::learning}]         = r.learning;
    j[std::string{jk::known_good}]       = r.known_good;
    j[std::string{jk::end_datetime}]     = r.end_datetime;
    j[std::string{jk::status_qualifier}] = r.status_qualifier;
    j[std::string{jk::board_number}]     = r.board_number;
    j[std::string{jk::parent_panel_id}]  = r.parent_panel_id;
    return j;
}
json to_json_record(const records::ProbeRecord& r) {
    json j = base_json(r);
    j[std::string{jk::raw}] = r.raw_payload;
    return j;
}

} // namespace

// =====================================================================
//  Public API
// =====================================================================

Record make_record(enums::Prefix p) {
    using P = enums::Prefix;
    if (enums::is_analog_test(p)) {
        records::AnalogTestRecord r{};
        r.prefix = p;
        return r;
    }
    switch (p) {
        case P::lim2:    return records::Lim2Record{};
        case P::lim3:    return records::Lim3Record{};
        case P::d_t:     return records::DigitalTestRecord{};
        case P::dpin:    return records::DevicePinRecord{};
        case P::d_pld:   return records::PldProgrammingRecord{};
        case P::cchk:    return records::ConnectCheckRecord{};
        case P::pchk:    return records::PolarityCheckRecord{};
        case P::tjet:    return records::TestJetRecord{};
        case P::indict:  return records::IndictmentRecord{};
        case P::pin:     return records::PinRecord{};
        case P::ts:      return records::ShortsTestRecord{};
        case P::ts_s:    return records::TsSourceRecord{};
        case P::ts_d:    return records::TsDestinationRecord{};
        case P::ts_o:    return records::TsOpenRecord{};
        case P::ts_p:    return records::TsPhantomRecord{};
        case P::batch:   return records::BatchRecord{};
        case P::block:   return records::BlockRecord{};
        case P::alm:     return records::AlarmRecord{};
        case P::aid:     return records::AlarmBoardRecord{};
        case P::array:   return records::ArrayRecord{};
        case P::netv:    return records::NetVerifyRecord{};
        case P::node:    return records::NodeListRecord{};
        case P::rpt:     return records::ReportRecord{};
        case P::retest:  return records::RetestRecord{};
        case P::pf:      return records::PfRecord{};
        case P::bs_s:    return records::BsShortRecord{};
        case P::bs_o:    return records::BsOpenRecord{};
        case P::bs_con:  return records::BoundaryScanRecord{};
        case P::btest:   return records::BTestRecord{};
        case P::prb:     return records::ProbeRecord{};
        default:         return std::monostate{};
    }
}

void parse_into(Record& rec, const std::vector<std::string>& fields) {
    std::visit([&](auto& r) { parse_record(r, fields); }, rec);
}

json to_json(const Record& rec) {
    return std::visit([](const auto& r) { return to_json_record(r); }, rec);
}

enums::Prefix prefix_of(const Record& rec) noexcept {
    return std::visit(overloaded{
        [](std::monostate) { return enums::Prefix::unknown; },
        [](const records::AnalogTestRecord& r) { return r.prefix; },  // dynamic
        [](const auto& r) { return std::remove_cvref_t<decltype(r)>::prefix; }  // static constexpr
    }, rec);
}

} // namespace ate::core
