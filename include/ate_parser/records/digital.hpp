/**
 * @file digital.hpp
 * @brief Digital-domain record value types.
 */
#pragma once

#include "ate_parser/enums/prefix.hpp"
#include "ate_parser/enums/test_status.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace ate::records {

struct DigitalTestRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::d_t;

    enums::DigitalTestStatus test_status = enums::DigitalTestStatus::passed;
    int                      test_substatus = 0;            // 6-bit flags
    std::optional<int>       failing_vector_number = 0;
    int                      pin_count = 0;
    std::string              test_designator;

    [[nodiscard]] std::vector<int> substatus_flags()                          const;
    void                           set_substatus_flags(const std::vector<int>& flags);

    [[nodiscard]] bool passed()      const noexcept { return test_status == enums::DigitalTestStatus::passed; }
    [[nodiscard]] bool failed()      const noexcept { return test_status == enums::DigitalTestStatus::failed; }
    [[nodiscard]] bool fatal_error() const noexcept { return test_status == enums::DigitalTestStatus::fatal_error; }
};

/// @DPIN: device + heterogeneous list of pin entries (strings or sub-arrays).
struct DevicePinRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::dpin;

    std::string                              device_name;
    nlohmann::json                           node_pin_list = nlohmann::json::array();
    std::optional<std::vector<std::string>>  thru_devnode_list;

    void add_node_pin(std::string_view node_id, std::string_view device_pin);
    void add_drive_thru_pair(std::string_view node_id, std::string_view device_id);
};

struct PldProgrammingRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::d_pld;

    std::string filename;
    std::string action;
    int         action_return_code = 0;
    std::string result_message;
    int         player_program_counter = 0;
};

struct ConnectCheckRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::cchk;

    enums::GenericTestStatus test_status = enums::GenericTestStatus::pass;
    int                      pin_count = 0;
    std::string              device_designator;
};

struct PolarityCheckRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::pchk;

    enums::GenericTestStatus test_status = enums::GenericTestStatus::pass;
    std::string              test_designator;
};

struct TestJetRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::tjet;

    enums::GenericTestStatus test_status = enums::GenericTestStatus::pass;
    int                      pin_count = 0;
    std::string              test_designator;
};

struct IndictmentRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::indict;

    std::string              technique;
    std::vector<std::string> device_list;
    std::optional<double>    est_resistance;
    std::optional<double>    est_capacitance;
    std::optional<double>    est_inductance;
    std::string              est_model;
};

struct PinRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::pin;

    std::optional<int>       pin_count;
    std::vector<std::string> pins;
};

} // namespace ate::records
