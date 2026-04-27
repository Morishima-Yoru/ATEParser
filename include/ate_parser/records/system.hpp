/**
 * @file system.hpp
 * @brief System / batch / boundary-scan record value types.
 */
#pragma once

#include "ate_parser/enums/prefix.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace ate::records {

struct BatchRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::batch;

    std::string uut_type;
    std::string uut_type_rev;
    int         fixture_id      = 0;
    int         testhead_number = 1;
    std::string testhead_type;
    std::string process_step;
    std::string batch_id;
    std::string operator_id;
    std::string controller;
    std::string testplan_id;
    std::string testplan_rev;
    std::string parent_panel_type;
    std::string parent_panel_type_rev;
    std::string version_label;
};

struct BlockRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::block;

    std::string block_designator;
    int         block_status = 0;
};

struct AlarmRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::alm;

    int                 alarm_type      = 0;
    bool                alarm_status    = false;
    std::string         datetime;
    std::string         board_type;
    std::string         board_rev;
    std::optional<int>  alarm_limit;
    std::optional<int>  detected_value;
    std::string         controller;
    int                 testhead_number = 1;
};

struct AlarmBoardRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::aid;

    std::string datetime;
    std::string serial;
};

struct ArrayRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::array;

    std::string         designator;
    int                 status = 0;
    std::optional<int>  failure_count;
    std::optional<int>  samples;
};

struct NetVerifyRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::netv;

    std::string datetime;
    std::string test_system;
    std::string repair_system;
    bool        source = false;
};

struct NodeListRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::node;

    int                      count = 0;
    std::vector<std::string> nodes;
};

struct ReportRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::rpt;

    std::string message;
};

struct RetestRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::retest;

    std::string datetime;
};

struct PfRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::pf;

    std::string         designator;
    std::optional<int>  test_status;
    std::optional<int>  total_pins;
};

struct BsShortRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::bs_s;

    std::string              cause;
    std::optional<int>       shorts_count;
    std::vector<std::string> node_list;
};

struct BsOpenRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::bs_o;

    std::string first_device_name;
    std::string first_device_pin;
    std::string second_device_name;
    std::string second_device_pin;
};

struct BoundaryScanRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::bs_con;

    std::string         test_designator;
    int                 status = 0;
    std::optional<int>  shorts_count;
    std::optional<int>  opens_count;
};

struct BTestRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::btest;

    std::string         board_id;
    int                 test_status      = 0;
    std::uint64_t       start_datetime   = 0;
    int                 duration         = 0;
    bool                multiple_test    = false;
    std::string         log_level;
    std::optional<int>  log_set;
    bool                learning         = false;
    bool                known_good       = false;
    std::uint64_t       end_datetime     = 0;
    std::string         status_qualifier;
    int                 board_number     = 0;
    std::string         parent_panel_id;
};

/// @PRB: probe record. Currently captured generically as raw fields.
struct ProbeRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::prb;
    std::string raw_payload;
};

} // namespace ate::records
