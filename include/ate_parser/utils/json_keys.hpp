/**
 * @file json_keys.hpp
 * @brief Constexpr JSON key constants used throughout serialization.
 *
 * Single source of truth for the JSON shape we emit. Keys are grouped by
 * category and exposed as `inline constexpr std::string_view` so they are
 * compile-time constants with no ODR issues.
 */
#pragma once

#include <string_view>

namespace ate::json_keys {

// Common
inline constexpr std::string_view prefix      = "prefix";
inline constexpr std::string_view raw         = "raw";
inline constexpr std::string_view subrecords  = "sub";

// Test status
inline constexpr std::string_view test_status        = "test_status";
inline constexpr std::string_view status             = "status";
inline constexpr std::string_view designator         = "designator";
inline constexpr std::string_view test_designator    = "test_designator";
inline constexpr std::string_view device_designator  = "device_designator";
inline constexpr std::string_view pin_count          = "pin_count";
inline constexpr std::string_view message            = "message";

// Analog
inline constexpr std::string_view measured_value      = "measured_value";
inline constexpr std::string_view subtest_designator  = "subtest_designator";

// Digital
inline constexpr std::string_view substatus       = "substatus";
inline constexpr std::string_view test_substatus  = "test_substatus";  // struct field name used in parse diagnostics
inline constexpr std::string_view failing_vector  = "failing_vector_number";

// Device pin
inline constexpr std::string_view device_name        = "device_name";
inline constexpr std::string_view node_pin_list      = "node_pin_list";
inline constexpr std::string_view thru_devnode_list  = "thru_devnode_list";

// PLD
inline constexpr std::string_view filename                 = "filename";
inline constexpr std::string_view action                   = "action";
inline constexpr std::string_view action_return_code       = "action_return_code";
inline constexpr std::string_view result_message           = "result_message";
inline constexpr std::string_view player_program_counter   = "player_program_counter";

// Shorts
inline constexpr std::string_view shorts_count       = "shorts_count";
inline constexpr std::string_view opens_count        = "opens_count";
inline constexpr std::string_view phantoms_count     = "phantoms_count";
inline constexpr std::string_view source_node        = "source_node";
inline constexpr std::string_view destination_node   = "destination_node";
inline constexpr std::string_view destinations_list  = "destinations_list";
inline constexpr std::string_view deviation          = "deviation";

// System
inline constexpr std::string_view uut_type               = "uut_type";
inline constexpr std::string_view uut_type_rev           = "uut_type_rev";
inline constexpr std::string_view fixture_id             = "fixture_id";
inline constexpr std::string_view testhead_number        = "testhead_number";
inline constexpr std::string_view testhead_type          = "testhead_type";
inline constexpr std::string_view process_step           = "process_step";
inline constexpr std::string_view batch_id               = "batch_id";
inline constexpr std::string_view operator_id            = "operator_id";
inline constexpr std::string_view controller             = "controller";
inline constexpr std::string_view testplan_id            = "testplan_id";
inline constexpr std::string_view testplan_rev           = "testplan_rev";
inline constexpr std::string_view parent_panel_type      = "parent_panel_type";
inline constexpr std::string_view parent_panel_type_rev  = "parent_panel_type_rev";
inline constexpr std::string_view version_label          = "version_label";
inline constexpr std::string_view block_designator       = "block_designator";
inline constexpr std::string_view block_status           = "block_status";

// Alarm
inline constexpr std::string_view alarm_type     = "alarm_type";
inline constexpr std::string_view alarm_status   = "alarm_status";
inline constexpr std::string_view datetime       = "datetime";
inline constexpr std::string_view board_type     = "board_type";
inline constexpr std::string_view board_rev      = "board_rev";
inline constexpr std::string_view alarm_limit    = "alarm_limit";
inline constexpr std::string_view detected_value = "detected_value";

// Limits
inline constexpr std::string_view high_limit    = "high_limit";
inline constexpr std::string_view low_limit     = "low_limit";
inline constexpr std::string_view nominal_value = "nominal_value";

// Misc
inline constexpr std::string_view nodes        = "nodes";
inline constexpr std::string_view technique    = "technique";
inline constexpr std::string_view device_list  = "device_list";
inline constexpr std::string_view device_count = "device_count";
inline constexpr std::string_view group_size   = "group_size";

// BTest
inline constexpr std::string_view board_id          = "board_id";
inline constexpr std::string_view start_datetime    = "start_datetime";
inline constexpr std::string_view duration          = "duration";
inline constexpr std::string_view multiple_test     = "multiple_test";
inline constexpr std::string_view log_level         = "log_level";
inline constexpr std::string_view log_set           = "log_set";
inline constexpr std::string_view learning          = "learning";
inline constexpr std::string_view known_good        = "known_good";
inline constexpr std::string_view end_datetime      = "end_datetime";
inline constexpr std::string_view status_qualifier  = "status_qualifier";
inline constexpr std::string_view board_number      = "board_number";
inline constexpr std::string_view parent_panel_id   = "parent_panel_id";

inline constexpr std::string_view serial_number  = "serial_number";
inline constexpr std::string_view failure_count  = "failure_count";
inline constexpr std::string_view samples        = "samples";
inline constexpr std::string_view test_system    = "test_system";
inline constexpr std::string_view repair_system  = "repair_system";
inline constexpr std::string_view source         = "source";
inline constexpr std::string_view count          = "count";
inline constexpr std::string_view total_pins     = "total_pins";
inline constexpr std::string_view cause          = "cause";
inline constexpr std::string_view node_list      = "node_list";

inline constexpr std::string_view first_device_name   = "first_device_name";
inline constexpr std::string_view first_device_pin    = "first_device_pin";
inline constexpr std::string_view second_device_name  = "second_device_name";
inline constexpr std::string_view second_device_pin   = "second_device_pin";

inline constexpr std::string_view est_resistance   = "est_resistance";
inline constexpr std::string_view est_capacitance  = "est_capacitance";
inline constexpr std::string_view est_inductance   = "est_inductance";
inline constexpr std::string_view est_model        = "est_model";

inline constexpr std::string_view pins  = "pins";

// Note record
inline constexpr std::string_view note_name    = "note_name";
inline constexpr std::string_view note_string  = "note_string";

// Export record
inline constexpr std::string_view key    = "key";
inline constexpr std::string_view field  = "field";

} // namespace ate::json_keys
