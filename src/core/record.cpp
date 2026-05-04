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
#include "ate_parser/utils/protocol_literals.hpp"
#include "ate_parser/utils/record_messages.hpp"
#include "ate_parser/utils/safe_conversion.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>

using nlohmann::json;

namespace ate::core {
namespace {

namespace jk      = ate::json_keys;
namespace proto   = ate::proto;
namespace rec_msg = ate::rec_msg;

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

void parse_record(std::monostate&, const std::vector<std::string_view>&) { /* no-op */ }

// ---- Limits ---------------------------------------------------------------
void parse_record(records::Lim2Record& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.high_limit = utils::parse_or_default<double>(f[0], jk::high_limit, enums::to_string_view(enums::Prefix::lim2), 0.0);
    if (f.size() > 1) r.low_limit  = utils::parse_or_default<double>(f[1], jk::low_limit,  enums::to_string_view(enums::Prefix::lim2), 0.0);
}
void parse_record(records::Lim3Record& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.nominal_value = utils::parse_or_default<double>(f[0], jk::nominal_value, enums::to_string_view(enums::Prefix::lim3), 0.0);
    if (f.size() > 1) r.high_limit    = utils::parse_or_default<double>(f[1], jk::high_limit,    enums::to_string_view(enums::Prefix::lim3), 0.0);
    if (f.size() > 2) r.low_limit     = utils::parse_or_default<double>(f[2], jk::low_limit,     enums::to_string_view(enums::Prefix::lim3), 0.0);
}

// ---- Analog test ----------------------------------------------------------
void parse_record(records::AnalogTestRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) {
        const int code = utils::parse_or_default<int>(f[0], jk::test_status, proto::k_analog_context, 0);
        try { r.test_status = enums::to_analog_status(code); }
        catch (const std::out_of_range&) {
            r.test_status = enums::AnalogTestStatus::failed_general;
            ATE_LOG_WARN(rec_msg::k_analog_unknown_status, code);
        }
    }
    if (f.size() > 1) {
        auto v = utils::parse_to<double>(f[1]);
        if (v) r.measured_value = *v;
    }
    if (f.size() > 2 && !f[2].empty()) {
        r.subtest_designator = strip_eol(std::string{f[2]});
        if (r.subtest_designator->empty()) r.subtest_designator.reset();
    }
}

// ---- Digital --------------------------------------------------------------
void parse_record(records::DigitalTestRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) {
        const int code = utils::parse_or_default<int>(f[0], jk::test_status, enums::to_string_view(enums::Prefix::d_t), 0);
        try { r.test_status = enums::to_digital_status(code); }
        catch (const std::out_of_range&) { r.test_status = enums::DigitalTestStatus::failed; }
    }
    if (f.size() > 1) r.test_substatus        = utils::parse_or_default<int>(f[1], jk::test_substatus, enums::to_string_view(enums::Prefix::d_t), 0);
    if (f.size() > 2) {
        auto v = utils::parse_to<int>(f[2]);
        r.failing_vector_number = v ? std::optional<int>(*v) : std::nullopt;
    }
    if (f.size() > 3) r.pin_count             = utils::parse_or_default<int>(f[3], jk::pin_count,      enums::to_string_view(enums::Prefix::d_t), 0);
    if (f.size() > 4) r.test_designator       = strip_eol(std::string{f[4]});
}

void parse_record(records::DevicePinRecord& r, const std::vector<std::string_view>& f) {
    r.device_name.clear();
    r.node_pin_list = json::array();

    if (!f.empty()) {
        auto pos = f[0].find('\\');
        r.device_name = (pos == std::string_view::npos) ? f[0] : f[0].substr(0, pos);
    }

    std::size_t i = 1;
    if (f.size() > i) {
        r.node_pin_list.push_back(f[i]);
        ++i;
    }
    if (f.size() > i) {
        std::string_view field = f[i];
        auto pos = field.find('\\');
        if (pos != std::string_view::npos) {
            auto base = field.substr(0, pos);
            r.node_pin_list.push_back(base);
            const int arr_len = utils::parse_or_default<int>(field.substr(pos + 1), jk::group_size, enums::to_string_view(enums::Prefix::dpin), 0);
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

void parse_record(records::PldProgrammingRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.filename               = f[0];
    if (f.size() > 1) r.action                 = f[1];
    if (f.size() > 2) r.action_return_code     = utils::parse_or_default<int>(f[2], jk::action_return_code,     enums::to_string_view(enums::Prefix::d_pld), 0);
    if (f.size() > 3) r.result_message         = f[3];
    if (f.size() > 4) r.player_program_counter = utils::parse_or_default<int>(f[4], jk::player_program_counter, enums::to_string_view(enums::Prefix::d_pld), 0);
}

void parse_record(records::ConnectCheckRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(utils::parse_or_default<int>(f[0], jk::test_status, enums::to_string_view(enums::Prefix::cchk), 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.pin_count         = utils::parse_or_default<int>(f[1], jk::pin_count, enums::to_string_view(enums::Prefix::cchk), 0);
    if (f.size() > 2) r.device_designator = strip_eol(std::string{f[2]});
}

void parse_record(records::PolarityCheckRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(utils::parse_or_default<int>(f[0], jk::test_status, enums::to_string_view(enums::Prefix::pchk), 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.test_designator = strip_eol(std::string{f[1]});
}

void parse_record(records::TestJetRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(utils::parse_or_default<int>(f[0], jk::test_status, enums::to_string_view(enums::Prefix::tjet), 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.pin_count       = utils::parse_or_default<int>(f[1], jk::pin_count, enums::to_string_view(enums::Prefix::tjet), 0);
    if (f.size() > 2) r.test_designator = strip_eol(std::string{f[2]});
}

void parse_record(records::IndictmentRecord& r, const std::vector<std::string_view>& f) {
    r.device_list.clear();
    r.technique.clear();

    int device_count = 0;
    if (!f.empty()) {
        auto pos = f[0].find('\\');
        if (pos != std::string_view::npos) {
            r.technique  = f[0].substr(0, pos);
            device_count = utils::parse_or_default<int>(f[0].substr(pos + 1), jk::device_count, enums::to_string_view(enums::Prefix::indict), 0);
        } else {
            r.technique = f[0];
        }
    }
    for (int i = 0; i < device_count && (1 + static_cast<std::size_t>(i)) < f.size(); ++i) {
        r.device_list.emplace_back(f[1 + i]);
    }
    const std::size_t next = 1 + static_cast<std::size_t>(device_count);
    if (f.size() > next)     { auto v = utils::parse_to<double>(f[next]);     r.est_resistance  = v ? std::optional<double>(*v) : std::nullopt; }
    if (f.size() > next + 1) { auto v = utils::parse_to<double>(f[next + 1]); r.est_capacitance = v ? std::optional<double>(*v) : std::nullopt; }
    if (f.size() > next + 2) { auto v = utils::parse_to<double>(f[next + 2]); r.est_inductance  = v ? std::optional<double>(*v) : std::nullopt; }
    if (f.size() > next + 3) r.est_model = strip_eol(std::string{f[next + 3]});
}

void parse_record(records::PinRecord& r, const std::vector<std::string_view>& f) {
    r.pins.clear();
    if (!f.empty()) {
        std::string_view first = f[0];
        auto pos = first.find('\\');
        auto count_str = (pos == std::string_view::npos) ? first : first.substr(pos + 1);
        auto v = utils::parse_to<int>(count_str);
        r.pin_count = v ? std::optional<int>(*v) : std::nullopt;
    }
    for (std::size_t i = 1; i < f.size(); ++i) r.pins.push_back(std::string{f[i]});
}

// ---- Shorts ---------------------------------------------------------------
void parse_record(records::ShortsTestRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) {
        try { r.test_status = enums::to_generic_status(utils::parse_or_default<int>(f[0], jk::test_status, enums::to_string_view(enums::Prefix::ts), 0)); }
        catch (const std::out_of_range&) { r.test_status = enums::GenericTestStatus::fail; }
    }
    if (f.size() > 1) r.shorts_count   = utils::parse_or_default<int>(f[1], jk::shorts_count,   enums::to_string_view(enums::Prefix::ts), 0);
    if (f.size() > 2) r.opens_count    = utils::parse_or_default<int>(f[2], jk::opens_count,    enums::to_string_view(enums::Prefix::ts), 0);
    if (f.size() > 3) r.phantoms_count = utils::parse_or_default<int>(f[3], jk::phantoms_count, enums::to_string_view(enums::Prefix::ts), 0);
    if (f.size() > 4) r.designator     = strip_eol(std::string{f[4]});
}

void parse_record(records::TsSourceRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.shorts_count   = utils::parse_or_default<int>(f[0], jk::shorts_count,   enums::to_string_view(enums::Prefix::ts_s), 0);
    if (f.size() > 1) r.phantoms_count = utils::parse_or_default<int>(f[1], jk::phantoms_count, enums::to_string_view(enums::Prefix::ts_s), 0);
    if (f.size() > 2) r.source_node    = strip_eol(std::string{f[2]});
}

void parse_record(records::TsDestinationRecord& r, const std::vector<std::string_view>& f) {
    r.destination_list.clear();
    // Field 0 is the "\count" prefix; pairs start at index 1.
    for (std::size_t i = 1; i + 1 < f.size(); i += 2) {
        const double dev = utils::parse_or_default<double>(f[i + 1], jk::deviation, enums::to_string_view(enums::Prefix::ts_d), 0.0);
        r.destination_list.emplace_back(f[i], dev);
    }
}

void parse_record(records::TsOpenRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.source_node      = f[0];
    if (f.size() > 1) r.destination_node = f[1];
    if (f.size() > 2) { auto v = utils::parse_to<double>(f[2]); r.deviation = v ? std::optional<double>(*v) : std::nullopt; }
}

void parse_record(records::TsPhantomRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) { auto v = utils::parse_to<double>(f[0]); r.deviation = v ? std::optional<double>(*v) : std::nullopt; }
}

// ---- System ---------------------------------------------------------------
void parse_record(records::BatchRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0)  r.uut_type              = f[0];
    if (f.size() > 1)  r.uut_type_rev          = f[1];
    if (f.size() > 2)  r.fixture_id            = utils::parse_or_default<int>(f[2], jk::fixture_id,      enums::to_string_view(enums::Prefix::batch), 0);
    if (f.size() > 3)  r.testhead_number       = utils::parse_or_default<int>(f[3], jk::testhead_number, enums::to_string_view(enums::Prefix::batch), 1);
    if (f.size() > 4)  r.testhead_type         = f[4];
    if (f.size() > 5)  r.process_step          = f[5];
    if (f.size() > 6)  r.batch_id              = f[6];
    if (f.size() > 7)  r.operator_id           = f[7];
    if (f.size() > 8)  r.controller            = f[8];
    if (f.size() > 9)  r.testplan_id           = f[9];
    if (f.size() > 10) r.testplan_rev          = f[10];
    if (f.size() > 11) r.parent_panel_type     = f[11];
    if (f.size() > 12) r.parent_panel_type_rev = f[12];
    if (f.size() > 13) r.version_label         = strip_eol(std::string{f[13]});
}

void parse_record(records::BlockRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.block_designator = f[0];
    if (f.size() > 1) r.block_status     = utils::parse_or_default<int>(f[1], jk::block_status, enums::to_string_view(enums::Prefix::block), 0);
}

void parse_record(records::AlarmRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.alarm_type      = utils::parse_or_default<int>(f[0], jk::alarm_type,   enums::to_string_view(enums::Prefix::alm), 1);
    if (f.size() > 1) r.alarm_status    = (utils::parse_or_default<int>(f[1], jk::alarm_status, enums::to_string_view(enums::Prefix::alm), 0) != 0);
    if (f.size() > 2) r.datetime        = f[2];
    if (f.size() > 3) r.board_type      = f[3];
    if (f.size() > 4) r.board_rev       = f[4];
    if (f.size() > 5) { auto v = utils::parse_to<int>(f[5]); r.alarm_limit    = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 6) { auto v = utils::parse_to<int>(f[6]); r.detected_value = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 7) r.controller      = f[7];
    if (f.size() > 8) r.testhead_number = utils::parse_or_default<int>(f[8], jk::testhead_number, enums::to_string_view(enums::Prefix::alm), 1);
}

void parse_record(records::AlarmBoardRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.datetime = f[0];
    if (f.size() > 1) r.serial   = strip_eol(std::string{f[1]});
}

void parse_record(records::ArrayRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.designator = f[0];
    if (f.size() > 1) r.status     = utils::parse_or_default<int>(f[1], jk::status, enums::to_string_view(enums::Prefix::array), 0);
    if (f.size() > 2) { auto v = utils::parse_to<int>(f[2]); r.failure_count = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 3) { auto v = utils::parse_to<int>(f[3]); r.samples       = v ? std::optional<int>(*v) : std::nullopt; }
}

void parse_record(records::NetVerifyRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.datetime      = f[0];
    if (f.size() > 1) r.test_system   = f[1];
    if (f.size() > 2) r.repair_system = f[2];
    if (f.size() > 3) r.source        = (utils::parse_or_default<int>(f[3], jk::source, enums::to_string_view(enums::Prefix::netv), 0) != 0);
}

void parse_record(records::NodeListRecord& r, const std::vector<std::string_view>& f) {
    r.nodes.clear();
    if (f.empty()) return;
    auto pos = f[0].find('\\');
    if (pos != std::string_view::npos) {
        r.count = utils::parse_or_default<int>(f[0].substr(pos + 1), jk::count, enums::to_string_view(enums::Prefix::node), 0);
    } else {
        r.count = 1;
        r.nodes.push_back(std::string{f[0]});
        return;
    }
    for (std::size_t i = 1; i < f.size(); ++i) r.nodes.push_back(std::string{f[i]});
}

void parse_record(records::ReportRecord& r, const std::vector<std::string_view>& f) {
    r.message.clear();
    for (std::size_t i = 0; i < f.size(); ++i) {
        if (i > 0) r.message.push_back('|');
        r.message += f[i];
    }
}

void parse_record(records::RetestRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.datetime = strip_eol(std::string{f[0]});
}

void parse_record(records::PfRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.designator  = f[0];
    if (f.size() > 1) { auto v = utils::parse_to<int>(f[1]); r.test_status = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 2) { auto v = utils::parse_to<int>(f[2]); r.total_pins  = v ? std::optional<int>(*v) : std::nullopt; }
}

void parse_record(records::BsShortRecord& r, const std::vector<std::string_view>& f) {
    r.node_list.clear();
    if (f.empty()) return;
    std::string_view first = f[0];
    auto pos = first.find('\\');
    if (pos != std::string_view::npos) {
        r.cause = first.substr(0, pos);
        auto v  = utils::parse_to<int>(first.substr(pos + 1));
        r.shorts_count = v ? std::optional<int>(*v) : std::nullopt;
    } else {
        r.cause.clear();
        auto v = utils::parse_to<int>(first);
        r.shorts_count = v ? std::optional<int>(*v) : std::nullopt;
    }
    for (std::size_t i = 1; i < f.size(); ++i) r.node_list.push_back(std::string{f[i]});
}

void parse_record(records::BsOpenRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.first_device_name  = f[0];
    if (f.size() > 1) r.first_device_pin   = f[1];
    if (f.size() > 2) r.second_device_name = f[2];
    if (f.size() > 3) r.second_device_pin  = f[3];
}

void parse_record(records::BoundaryScanRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0) r.test_designator = f[0];
    if (f.size() > 1) r.status          = utils::parse_or_default<int>(f[1], jk::status, enums::to_string_view(enums::Prefix::bs_con), 0);
    if (f.size() > 2) { auto v = utils::parse_to<int>(f[2]); r.shorts_count = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 3) { auto v = utils::parse_to<int>(f[3]); r.opens_count  = v ? std::optional<int>(*v) : std::nullopt; }
}

void parse_record(records::BTestRecord& r, const std::vector<std::string_view>& f) {
    if (f.size() > 0)  r.board_id         = f[0];
    if (f.size() > 1)  r.test_status      = utils::parse_or_default<int>(f[1], jk::test_status,    enums::to_string_view(enums::Prefix::btest), 0);
    if (f.size() > 2)  r.start_datetime   = utils::parse_or_default<unsigned long long>(f[2], jk::start_datetime, enums::to_string_view(enums::Prefix::btest), 0ull);
    if (f.size() > 3)  r.duration         = utils::parse_or_default<int>(f[3], jk::duration,        enums::to_string_view(enums::Prefix::btest), 0);
    if (f.size() > 4)  r.multiple_test    = utils::parse_bool_or_default(f[4], jk::multiple_test,  enums::to_string_view(enums::Prefix::btest), false);
    if (f.size() > 5)  r.log_level        = f[5];
    if (f.size() > 6)  { auto v = utils::parse_to<int>(f[6]); r.log_set = v ? std::optional<int>(*v) : std::nullopt; }
    if (f.size() > 7)  r.learning         = utils::parse_bool_or_default(f[7], jk::learning,   enums::to_string_view(enums::Prefix::btest), false);
    if (f.size() > 8)  r.known_good       = utils::parse_bool_or_default(f[8], jk::known_good, enums::to_string_view(enums::Prefix::btest), false);
    if (f.size() > 9)  r.end_datetime     = utils::parse_or_default<unsigned long long>(f[9], jk::end_datetime,   enums::to_string_view(enums::Prefix::btest), 0ull);
    if (f.size() > 10) r.status_qualifier = f[10];
    if (f.size() > 11) r.board_number     = utils::parse_or_default<int>(f[11], jk::board_number, enums::to_string_view(enums::Prefix::btest), 0);
    if (f.size() > 12) r.parent_panel_id  = strip_eol(std::string{f[12]});
}

void parse_record(records::ProbeRecord& r, const std::vector<std::string_view>& f) {
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
    return json{ {jk::prefix, enums::to_string_view(r.prefix)} };
}

json to_json_record(std::monostate)              { return json::object(); }

json to_json_record(const records::Lim2Record& r) {
    json j = base_json(r);
    j[jk::high_limit] = r.high_limit;
    j[jk::low_limit]  = r.low_limit;
    return j;
}
json to_json_record(const records::Lim3Record& r) {
    json j = base_json(r);
    j[jk::nominal_value] = r.nominal_value;
    j[jk::high_limit]    = r.high_limit;
    j[jk::low_limit]     = r.low_limit;
    return j;
}
json to_json_record(const records::AnalogTestRecord& r) {
    json j {
        {jk::prefix,      enums::to_string_view(r.prefix)},
        {jk::test_status, static_cast<int>(r.test_status)},
    };
    if (r.measured_value)     j[jk::measured_value]     = *r.measured_value;
    if (r.subtest_designator && !r.subtest_designator->empty())
                              j[jk::subtest_designator] = *r.subtest_designator;
    return j;
}

json to_json_record(const records::DigitalTestRecord& r) {
    json j = base_json(r);
    j[jk::test_status]     = static_cast<int>(r.test_status);
    j[jk::substatus]       = r.test_substatus;
    j[jk::failing_vector]  = opt_to_json(r.failing_vector_number);
    j[jk::pin_count]       = r.pin_count;
    j[jk::test_designator] = r.test_designator;
    return j;
}
json to_json_record(const records::DevicePinRecord& r) {
    json j = base_json(r);
    j[jk::device_name]   = r.device_name;
    j[jk::node_pin_list] = r.node_pin_list;
    if (r.thru_devnode_list) j[jk::thru_devnode_list] = *r.thru_devnode_list;
    return j;
}
json to_json_record(const records::PldProgrammingRecord& r) {
    json j = base_json(r);
    j[jk::filename]               = r.filename;
    j[jk::action]                 = r.action;
    j[jk::action_return_code]     = r.action_return_code;
    j[jk::result_message]         = r.result_message;
    j[jk::player_program_counter] = r.player_program_counter;
    return j;
}
json to_json_record(const records::ConnectCheckRecord& r) {
    json j = base_json(r);
    j[jk::status]            = static_cast<int>(r.test_status);
    j[jk::pin_count]         = r.pin_count;
    j[jk::device_designator] = r.device_designator;
    return j;
}
json to_json_record(const records::PolarityCheckRecord& r) {
    json j = base_json(r);
    j[jk::status]     = static_cast<int>(r.test_status);
    j[jk::designator] = r.test_designator;
    return j;
}
json to_json_record(const records::TestJetRecord& r) {
    json j = base_json(r);
    j[jk::status]     = static_cast<int>(r.test_status);
    j[jk::pin_count]  = r.pin_count;
    j[jk::designator] = r.test_designator;
    return j;
}
json to_json_record(const records::IndictmentRecord& r) {
    json j = base_json(r);
    j[jk::technique]       = r.technique;
    j[jk::device_list]     = r.device_list;
    j[jk::est_resistance]  = opt_to_json(r.est_resistance);
    j[jk::est_capacitance] = opt_to_json(r.est_capacitance);
    j[jk::est_inductance]  = opt_to_json(r.est_inductance);
    j[jk::est_model]       = r.est_model;
    return j;
}
json to_json_record(const records::PinRecord& r) {
    json j = base_json(r);
    j[jk::pin_count] = opt_to_json(r.pin_count);
    j[jk::pins]      = r.pins;
    return j;
}

// Shorts
json to_json_record(const records::ShortsTestRecord& r) {
    json j = base_json(r);
    j[jk::test_status]    = static_cast<int>(r.test_status);
    j[jk::shorts_count]   = r.shorts_count;
    j[jk::opens_count]    = r.opens_count;
    j[jk::phantoms_count] = r.phantoms_count;
    j[jk::designator]     = r.designator.empty() ? json(nullptr) : json(r.designator);
    return j;
}
json to_json_record(const records::TsSourceRecord& r) {
    json j = base_json(r);
    j[jk::shorts_count]   = r.shorts_count;
    j[jk::phantoms_count] = r.phantoms_count;
    j[jk::source_node]    = r.source_node;
    return j;
}
json to_json_record(const records::TsDestinationRecord& r) {
    json arr = json::array();
    for (auto& p : r.destination_list) arr.push_back({p.first, p.second});
    json j = base_json(r);
    j[jk::destinations_list] = arr;
    return j;
}
json to_json_record(const records::TsOpenRecord& r) {
    json j = base_json(r);
    j[jk::source_node]      = r.source_node;
    j[jk::destination_node] = r.destination_node;
    j[jk::deviation]        = opt_to_json(r.deviation);
    return j;
}
json to_json_record(const records::TsPhantomRecord& r) {
    json j = base_json(r);
    j[jk::deviation] = opt_to_json(r.deviation);
    return j;
}

// System
json to_json_record(const records::BatchRecord& r) {
    json j = base_json(r);
    j[jk::uut_type]              = r.uut_type;
    j[jk::uut_type_rev]          = r.uut_type_rev;
    j[jk::fixture_id]            = r.fixture_id;
    j[jk::testhead_number]       = r.testhead_number;
    j[jk::testhead_type]         = r.testhead_type;
    j[jk::process_step]          = r.process_step;
    j[jk::batch_id]              = r.batch_id;
    j[jk::operator_id]           = r.operator_id;
    j[jk::controller]            = r.controller;
    j[jk::testplan_id]           = r.testplan_id;
    j[jk::testplan_rev]          = r.testplan_rev;
    j[jk::parent_panel_type]     = r.parent_panel_type;
    j[jk::parent_panel_type_rev] = r.parent_panel_type_rev;
    j[jk::version_label]         = r.version_label;
    return j;
}
json to_json_record(const records::BlockRecord& r) {
    json j = base_json(r);
    j[jk::block_designator] = r.block_designator;
    j[jk::block_status]     = r.block_status;
    return j;
}
json to_json_record(const records::AlarmRecord& r) {
    json j = base_json(r);
    j[jk::alarm_type]      = r.alarm_type;
    j[jk::alarm_status]    = r.alarm_status;
    j[jk::datetime]        = r.datetime;
    j[jk::board_type]      = r.board_type;
    j[jk::board_rev]       = r.board_rev;
    j[jk::alarm_limit]     = opt_to_json(r.alarm_limit);
    j[jk::detected_value]  = opt_to_json(r.detected_value);
    j[jk::controller]      = r.controller;
    j[jk::testhead_number] = r.testhead_number;
    return j;
}
json to_json_record(const records::AlarmBoardRecord& r) {
    json j = base_json(r);
    j[jk::datetime]      = r.datetime;
    j[jk::serial_number] = r.serial;
    return j;
}
json to_json_record(const records::ArrayRecord& r) {
    json j = base_json(r);
    j[jk::designator]    = r.designator;
    j[jk::status]        = r.status;
    j[jk::failure_count] = opt_to_json(r.failure_count);
    j[jk::samples]       = opt_to_json(r.samples);
    return j;
}
json to_json_record(const records::NetVerifyRecord& r) {
    json j = base_json(r);
    j[jk::datetime]      = r.datetime;
    j[jk::test_system]   = r.test_system;
    j[jk::repair_system] = r.repair_system;
    j[jk::source]        = r.source;
    return j;
}
json to_json_record(const records::NodeListRecord& r) {
    json j = base_json(r);
    j[jk::count] = r.count;
    j[jk::nodes] = r.nodes;
    return j;
}
json to_json_record(const records::ReportRecord& r) {
    json j = base_json(r);
    j[jk::message] = r.message;
    return j;
}
json to_json_record(const records::RetestRecord& r) {
    json j = base_json(r);
    j[jk::datetime] = r.datetime;
    return j;
}
json to_json_record(const records::PfRecord& r) {
    json j = base_json(r);
    j[jk::designator]  = r.designator;
    j[jk::test_status] = opt_to_json(r.test_status);
    j[jk::total_pins]  = opt_to_json(r.total_pins);
    return j;
}
json to_json_record(const records::BsShortRecord& r) {
    json j = base_json(r);
    j[jk::cause]        = r.cause;
    j[jk::shorts_count] = opt_to_json(r.shorts_count);
    j[jk::node_list]    = r.node_list;
    return j;
}
json to_json_record(const records::BsOpenRecord& r) {
    json j = base_json(r);
    j[jk::first_device_name]   = r.first_device_name;
    j[jk::first_device_pin]    = r.first_device_pin;
    j[jk::second_device_name]  = r.second_device_name.empty() ? json(nullptr) : json(r.second_device_name);
    j[jk::second_device_pin]   = r.second_device_pin.empty()  ? json(nullptr) : json(r.second_device_pin);
    return j;
}
json to_json_record(const records::BoundaryScanRecord& r) {
    json j = base_json(r);
    j[jk::test_designator] = r.test_designator;
    j[jk::status]          = r.status;
    j[jk::shorts_count]    = opt_to_json(r.shorts_count);
    j[jk::opens_count]     = opt_to_json(r.opens_count);
    return j;
}
json to_json_record(const records::BTestRecord& r) {
    json j = base_json(r);
    j[jk::board_id]         = r.board_id;
    j[jk::test_status]      = r.test_status;
    j[jk::start_datetime]   = r.start_datetime;
    j[jk::duration]         = r.duration;
    j[jk::multiple_test]    = r.multiple_test;
    j[jk::log_level]        = r.log_level;
    j[jk::log_set]          = opt_to_json(r.log_set);
    j[jk::learning]         = r.learning;
    j[jk::known_good]       = r.known_good;
    j[jk::end_datetime]     = r.end_datetime;
    j[jk::status_qualifier] = r.status_qualifier;
    j[jk::board_number]     = r.board_number;
    j[jk::parent_panel_id]  = r.parent_panel_id;
    return j;
}
json to_json_record(const records::ProbeRecord& r) {
    json j = base_json(r);
    j[jk::raw] = r.raw_payload;
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

void parse_into(Record& rec, const std::vector<std::string_view>& fields) {
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
