/**
 * @file SystemRecords.hpp
 * @brief Defines system-level and batch record structures for Keysight i3070 ICT logs
 * 
 * This header contains all top-level log record structures that manage batch, block,
 * alarm, identification, network verification, node lists, report messages, and retest
 * records as defined by the i3070 Log Record Format.
 * 
 * @author Keysight Log Parser Team
 * @version 1.0
 * @date 2025-06-19
 */

#pragma once

#include "../core/LogRecord.hpp"
#include "../enums/LogRecordPrefix.hpp"
#include <string>
#include <vector>
#include <optional>
#include <nlohmann/json.hpp>

namespace i3070::records {

/**
 * @struct BatchRecord
 * @brief Log record for identifying a batch of boards (@BATCH)
 */
struct BatchRecord : public core::LogRecord {
    std::string uut_type;               ///< Unit Under Test type [1]
    std::string uut_type_rev;           ///< UUT type revision [1]
    int fixture_id;                     ///< Fixture identifier [1]
    int testhead_number;                ///< Testhead number [1]
    std::string testhead_type;          ///< Testhead type [1]
    std::string process_step;           ///< Manufacturing process step [1]
    std::string batch_id;               ///< Batch identifier [1]
    std::string operator_id;            ///< Operator identifier [1]
    std::string controller;             ///< Controller name [1]
    std::string testplan_id;            ///< Testplan identifier [1]
    std::string testplan_rev;           ///< Testplan revision [1]
    std::string parent_panel_type;      ///< Parent panel type [1]
    std::string parent_panel_type_rev;  ///< Parent panel revision [1]
    std::string version_label;          ///< Version label (optional) [1]

    BatchRecord()
      : LogRecord(enums::LogRecordPrefix::BATCH),
        fixture_id(0),
        testhead_number(1) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct BlockRecord
 * @brief Log record for identifying a test block (@BLOCK)
 */
struct BlockRecord : public core::LogRecord {
    std::string block_designator; ///< Block name or identifier [1]
    int block_status;             ///< Block pass/fail status [1]

    BlockRecord()
      : LogRecord(enums::LogRecordPrefix::BLOCK),
        block_status(0) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct AlarmRecord
 * @brief Log record for real-time alarms (@ALM)
 */
struct AlarmRecord : public core::LogRecord {
    int alarm_type;            ///< 1=consecutive failure, 2=sample yield, 3=overall yield [1]
    bool alarm_status;         ///< Alarm on/off flag [1]
    std::string datetime;      ///< Detection timestamp YYMMDDHHMMSS [1]
    std::string board_type;    ///< Board type string [1]
    std::string board_rev;     ///< Board revision string [1]
    std::optional<int> alarm_limit;    ///< Alarm limit [1]
    std::optional<int> detected_value; ///< Detected value [1]
    std::string controller;            ///< Controller name [1]
    int testhead_number;               ///< Testhead number [1]

    AlarmRecord() : LogRecord(enums::LogRecordPrefix::ALM), alarm_type(0), alarm_status(false), testhead_number(1) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct AlarmBoardRecord
 * @brief Log record for identifying board causing alarm (@AID)
 */
struct AlarmBoardRecord : public core::LogRecord {
    std::string datetime;
    std::string serial;

    AlarmBoardRecord() : LogRecord(enums::LogRecordPrefix::AID) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct ArrayRecord
 * @brief Log record for array testing (@ARRAY)
 */
struct ArrayRecord : public core::LogRecord {
    std::string designator;
    int status;
    std::optional<int> failure_count;
    std::optional<int> samples;

    ArrayRecord() : LogRecord(enums::LogRecordPrefix::ARRAY), status(0) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct NetVerifyRecord
 * @brief Log record for network verification (@NETV)
 */
struct NetVerifyRecord : public core::LogRecord {
    std::string datetime;
    std::string test_system;
    std::string repair_system;
    bool source;

    NetVerifyRecord() : LogRecord(enums::LogRecordPrefix::NETV), source(false) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct NodeListRecord
 * @brief Log record for node lists (@NODE)
 */
struct NodeListRecord : public core::LogRecord {
    int count;
    std::vector<std::string> nodes;

    NodeListRecord() : LogRecord(enums::LogRecordPrefix::NODE), count(0) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct ReportRecord
 * @brief Log record for report messages (@RPT)
 */
struct ReportRecord : public core::LogRecord {
    std::string message;

    ReportRecord() : LogRecord(enums::LogRecordPrefix::RPT) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct RetestRecord
 * @brief Log record for retest information (@RETEST)
 */
struct RetestRecord : public core::LogRecord {
    std::string datetime;

    RetestRecord() : LogRecord(enums::LogRecordPrefix::RETEST) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct PFRecord
 * @brief Log record for pins fail (@PF)
 */
struct PFRecord : public core::LogRecord {
    std::string designator;
    std::optional<int> test_status;
    std::optional<int> total_pins;

    PFRecord() : LogRecord(enums::LogRecordPrefix::PF) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct BsShortRecord
 * @brief Log record for boundary scan short (@BS-S)
 */
struct BsShortRecord : public core::LogRecord {
    std::string cause;
    std::optional<int> shorts_count;
    std::vector<std::string> node_list;

    BsShortRecord() : LogRecord(enums::LogRecordPrefix::BS_S) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct BsOpenRecord
 * @brief Log record for boundary scan open (@BS-O)
 */
struct BsOpenRecord : public core::LogRecord {
    std::string first_device_name;
    std::string first_device_pin;
    std::string second_device_name;
    std::string second_device_pin;

    BsOpenRecord() : LogRecord(enums::LogRecordPrefix::BS_O) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct BoundaryScanRecord
 * @brief Log record for boundary scan connect (@BS-CON)
 */
struct BoundaryScanRecord : public core::LogRecord {
    std::string test_designator;
    int status;
    std::optional<int> shorts_count;
    std::optional<int> opens_count;

    BoundaryScanRecord() : LogRecord(enums::LogRecordPrefix::BS_CON), status(0) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct BTestRecord
 * @brief Log record for board test (@BTEST)
 */
struct BTestRecord : public core::LogRecord {
    std::string board_id;
    int test_status;
    unsigned long long start_datetime;
    int duration;
    bool multiple_test;
    std::string log_level;
    std::optional<int> log_set;
    bool learning;
    bool known_good;
    unsigned long long end_datetime;
    std::string status_qualifier;
    int board_number;
    std::string parent_panel_id;

    BTestRecord() : LogRecord(enums::LogRecordPrefix::BTEST), test_status(0), start_datetime(0), duration(0), multiple_test(false), learning(false), known_good(false), end_datetime(0), board_number(0) {}

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

} // namespace i3070::records
