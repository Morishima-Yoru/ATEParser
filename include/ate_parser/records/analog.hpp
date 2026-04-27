/**
 * @file analog.hpp
 * @brief Analog test record value types (no inheritance, no virtuals).
 *
 * Each struct mirrors a Keysight i3070 ICT analog log record. They are
 * plain value types -- copyable, movable, comparable. JSON serialization
 * and field parsing are done through free functions in `record.hpp`.
 */
#pragma once

#include "ate_parser/enums/prefix.hpp"
#include "ate_parser/enums/test_status.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace ate::records {

// ---- Limit records --------------------------------------------------------

struct Lim2Record {
    static constexpr enums::Prefix prefix = enums::Prefix::lim2;

    double high_limit = 0.0;
    double low_limit  = 0.0;

    [[nodiscard]] bool   has_nominal()                    const noexcept { return false; }
    [[nodiscard]] double range()                          const noexcept { return high_limit - low_limit; }
    [[nodiscard]] double center()                         const noexcept { return (high_limit + low_limit) / 2.0; }
    [[nodiscard]] bool   within(double measured)          const noexcept { return measured >= low_limit && measured <= high_limit; }
};

struct Lim3Record {
    static constexpr enums::Prefix prefix = enums::Prefix::lim3;

    double nominal_value = 0.0;
    double high_limit    = 0.0;
    double low_limit     = 0.0;

    [[nodiscard]] bool   has_nominal()                    const noexcept { return true; }
    [[nodiscard]] double positive_tolerance()             const noexcept { return high_limit - nominal_value; }
    [[nodiscard]] double negative_tolerance()             const noexcept { return nominal_value - low_limit; }
    [[nodiscard]] bool   has_symmetric_tolerance()        const noexcept;
    [[nodiscard]] bool   within(double measured)          const noexcept { return measured >= low_limit && measured <= high_limit; }
    [[nodiscard]] double percent_deviation(double measured) const;  ///< throws std::domain_error if nominal == 0
};

// ---- Analog test record ---------------------------------------------------

/// Common payload of every analog component test (@A-CAP, @A-RES, ...).
/// The component family is identified by the `prefix` member, set at
/// construction by the factory.
struct AnalogTestRecord {
    enums::Prefix                       prefix             = enums::Prefix::a_res;
    enums::AnalogTestStatus             test_status        = enums::AnalogTestStatus::passed;
    std::optional<double>               measured_value;
    std::optional<std::string>          subtest_designator;

    [[nodiscard]] bool        passed()            const noexcept { return test_status == enums::AnalogTestStatus::passed; }
    [[nodiscard]] bool        failed()            const noexcept { return test_status != enums::AnalogTestStatus::passed; }
    [[nodiscard]] bool        aborted()           const noexcept { return test_status == enums::AnalogTestStatus::aborted_by_operator; }
    [[nodiscard]] std::string status_description() const         { return enums::to_string(test_status); }
};

[[nodiscard]] bool is_analog_test_prefix(enums::Prefix p) noexcept;
[[nodiscard]] bool expects_lim3(enums::Prefix p);  ///< throws std::invalid_argument for non-analog prefixes

} // namespace ate::records
