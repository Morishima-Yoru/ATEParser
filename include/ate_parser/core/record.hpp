/**
 * @file record.hpp
 * @brief `Record` -- a std::variant of every concrete record type.
 *
 * This header is the centerpiece of the type-driven design: instead of a
 * polymorphic class hierarchy with `virtual` methods, we use a single
 * `std::variant<...>` that holds any concrete record value, and a few
 * free functions that dispatch with `std::visit`.
 *
 * Available free functions:
 *   * `make_record(prefix)` -- construct a default record of the right type.
 *   * `parse_into(record, fields)` -- populate from raw string fields.
 *   * `to_json(record)` -- serialize to nlohmann::json.
 *   * `prefix_of(record)` -- lookup the record's prefix at runtime.
 *
 * All concrete record types live under `ate::records` and have a
 * `static constexpr Prefix prefix` member, used by the visit machinery.
 */
#pragma once

#include "ate_parser/enums/prefix.hpp"
#include "ate_parser/records/analog.hpp"
#include "ate_parser/records/digital.hpp"
#include "ate_parser/records/shorts.hpp"
#include "ate_parser/records/system.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <variant>
#include <vector>

namespace ate::core {

/// `std::monostate` slot represents an unknown / not-yet-decoded record.
using Record = std::variant<
    std::monostate,
    // Analog
    records::AnalogTestRecord,
    records::Lim2Record, records::Lim3Record,
    // Digital
    records::DigitalTestRecord, records::DevicePinRecord,
    records::PldProgrammingRecord, records::ConnectCheckRecord,
    records::PolarityCheckRecord, records::TestJetRecord,
    records::IndictmentRecord,    records::PinRecord,
    // Shorts
    records::ShortsTestRecord,    records::TsSourceRecord,
    records::TsDestinationRecord, records::TsOpenRecord,
    records::TsPhantomRecord,
    // System
    records::BatchRecord,    records::BlockRecord,
    records::AlarmRecord,    records::AlarmBoardRecord,
    records::ArrayRecord,    records::NetVerifyRecord,
    records::NodeListRecord, records::ReportRecord,
    records::RetestRecord,   records::PfRecord,
    records::BsShortRecord,  records::BsOpenRecord,
    records::BoundaryScanRecord, records::BTestRecord,
    records::ProbeRecord
>;

/// Canonical "overloaded" lambda set used with std::visit.
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/// Construct a default-initialized record matching @p prefix.
/// For analog test prefixes, the analog `prefix` field is set so that
/// the variant carries the exact subtype information.
[[nodiscard]] Record make_record(enums::Prefix prefix);

/// Parse raw string fields into @p record. Behaviour for an unknown record
/// (`std::monostate`) is a no-op (with a debug log).
void parse_into(Record& record, const std::vector<std::string>& fields);

/// Serialize @p record to JSON. Empty for `std::monostate`.
[[nodiscard]] nlohmann::json to_json(const Record& record);

/// Returns the runtime `Prefix` of @p record.
[[nodiscard]] enums::Prefix prefix_of(const Record& record) noexcept;

/// Returns true iff @p record holds `std::monostate`.
[[nodiscard]] inline bool is_unknown(const Record& r) noexcept {
    return std::holds_alternative<std::monostate>(r);
}

} // namespace ate::core
