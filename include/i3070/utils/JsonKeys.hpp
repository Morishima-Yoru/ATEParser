#ifndef I3070_CORE_JSONKEYS_HPP
#define I3070_CORE_JSONKEYS_HPP

// JSON output key name definitions
// Use macros for unified management and modification

// ===================== Basic Fields =====================
#define JSON_KEY_PREFIX        "prefix"
#define JSON_KEY_RAW           "raw"
#define JSON_KEY_SUBRECORDS    "sub"

// ===================== Common Test Fields =====================
#define JSON_KEY_TEST_STATUS   "test_status"
#define JSON_KEY_STATUS        "status"
#define JSON_KEY_DESIGNATOR    "designator"
#define JSON_KEY_TEST_DESIGNATOR "test_designator"
#define JSON_KEY_DEVICE_DESIGNATOR "device_designator"
#define JSON_KEY_PIN_COUNT     "pin_count"
#define JSON_KEY_MESSAGE       "message"

// ===================== Analog Test Record Fields =====================
#define JSON_KEY_MEASURED_VALUE "measured_value"
#define JSON_KEY_SUBTEST_DESIGNATOR "subtest_designator"
#define JSON_KEY_COMPONENT_TYPE "component_type"

// ===================== Digital Test Record Fields =====================
#define JSON_KEY_SUBSTATUS     "substatus"
#define JSON_KEY_FAILING_VECTOR "failing_vector_number"

// ===================== Device Pin Record Fields =====================
#define JSON_KEY_DEVICE_NAME   "device_name"
#define JSON_KEY_NODE_PIN_LIST "node_pin_list"
#define JSON_KEY_THRU_DEVNODE_LIST "thru_devnode_list"

// ===================== PLD Programming Record Fields =====================
#define JSON_KEY_FILENAME      "filename"
#define JSON_KEY_ACTION        "action"
#define JSON_KEY_ACTION_RETURN_CODE "action_return_code"
#define JSON_KEY_RESULT_MESSAGE "result_message"
#define JSON_KEY_PLAYER_PROGRAM_COUNTER "player_program_counter"

// ===================== Export Record Fields =====================
#define JSON_KEY_KEY           "key"
#define JSON_KEY_FIELD         "field"

// ===================== Note Record Fields =====================
#define JSON_KEY_NOTE_NAME     "note_name"
#define JSON_KEY_NOTE_STRING   "note_string"

// ===================== Shorts Test Record Fields =====================
#define JSON_KEY_SHORTS_COUNT  "shorts_count"
#define JSON_KEY_OPENS_COUNT   "opens_count"
#define JSON_KEY_PHANTOMS_COUNT "phantoms_count"
#define JSON_KEY_SOURCE_NODE   "source_node"
#define JSON_KEY_DESTINATION_NODE "destination_node"
#define JSON_KEY_DESTINATIONS_LIST  "destinations_list"
#define JSON_KEY_DEVIATION     "deviation"
#define JSON_KEY_NODE          "node"

// ===================== System Record Fields =====================
#define JSON_KEY_UUT_TYPE      "uut_type"
#define JSON_KEY_UUT_TYPE_REV  "uut_type_rev"
#define JSON_KEY_FIXTURE_ID    "fixture_id"
#define JSON_KEY_TESTHEAD_NUMBER "testhead_number"
#define JSON_KEY_TESTHEAD_TYPE "testhead_type"
#define JSON_KEY_PROCESS_STEP  "process_step"
#define JSON_KEY_BATCH_ID      "batch_id"
#define JSON_KEY_OPERATOR_ID   "operator_id"
#define JSON_KEY_CONTROLLER    "controller"
#define JSON_KEY_TESTPLAN_ID   "testplan_id"
#define JSON_KEY_TESTPLAN_REV  "testplan_rev"
#define JSON_KEY_PARENT_PANEL_TYPE "parent_panel_type"
#define JSON_KEY_PARENT_PANEL_TYPE_REV "parent_panel_type_rev"
#define JSON_KEY_VERSION_LABEL "version_label"
#define JSON_KEY_BLOCK_DESIGNATOR "block_designator"
#define JSON_KEY_BLOCK_STATUS     "block_status"

// ===================== Alarm Record Fields =====================
#define JSON_KEY_ALARM_TYPE    "alarm_type"
#define JSON_KEY_ALARM_STATUS  "alarm_status"
#define JSON_KEY_DATETIME      "datetime"
#define JSON_KEY_BOARD_TYPE    "board_type"
#define JSON_KEY_BOARD_REV     "board_rev"
#define JSON_KEY_ALARM_LIMIT   "alarm_limit"
#define JSON_KEY_DETECTED_VALUE "detected_value"

// ===================== Limit Record Fields =====================
#define JSON_KEY_LIMIT_TYPE    "limit_type"
#define JSON_KEY_HIGH_LIMIT    "high_limit"
#define JSON_KEY_LOW_LIMIT     "low_limit"
#define JSON_KEY_NOMINAL_VALUE "nominal_value"

// ===================== Common Fields =====================
#define JSON_KEY_NODES         "nodes"
#define JSON_KEY_TECHNIQUE     "technique"
#define JSON_KEY_DEVICE_LIST   "device_list"

// ===================== BTestRecord Fields =====================
#define JSON_KEY_BOARD_ID         "board_id"
#define JSON_KEY_TEST_STATUS      "test_status"
#define JSON_KEY_START_DATETIME   "start_datetime"
#define JSON_KEY_DURATION         "duration"
#define JSON_KEY_MULTIPLE_TEST    "multiple_test"
#define JSON_KEY_LOG_LEVEL        "log_level"
#define JSON_KEY_LOG_SET          "log_set"
#define JSON_KEY_LEARNING         "learning"
#define JSON_KEY_KNOWN_GOOD       "known_good"
#define JSON_KEY_END_DATETIME     "end_datetime"
#define JSON_KEY_STATUS_QUALIFIER "status_qualifier"
#define JSON_KEY_BOARD_NUMBER     "board_number"
#define JSON_KEY_PARENT_PANEL_ID  "parent_panel_id"

// ===================== PFRecord Fields =====================
#define JSON_KEY_PF_TOTAL_PINS  "total_pins"
#define JSON_KEY_PF_FAIL_COUNT  "fail_count"
#define JSON_KEY_PF_FAIL_TYPE   "fail_type"
#define JSON_KEY_TOTAL_PINS     "total_pins"

#define JSON_KEY_PINS           "pins"

// ===================== Array Record Fields =====================
#define JSON_KEY_FAILURE_COUNT "failure_count"
#define JSON_KEY_SAMPLES       "samples"

// ===================== NetVerify Record Fields =====================
#define JSON_KEY_TEST_SYSTEM   "test_system"
#define JSON_KEY_REPAIR_SYSTEM "repair_system"
#define JSON_KEY_SOURCE        "source"

// ===================== NodeList Record Fields =====================
#define JSON_KEY_COUNT         "count"

// ===================== BoundaryScan Record Fields =====================
#define JSON_KEY_SHORTS_COUNT  "shorts_count"
#define JSON_KEY_OPENS_COUNT   "opens_count"
#define JSON_KEY_CAUSE         "cause"
#define JSON_KEY_NODE_LIST     "node_list"
#define JSON_KEY_FIRST_DEVICE_NAME  "first_device_name"
#define JSON_KEY_FIRST_DEVICE_PIN   "first_device_pin"
#define JSON_KEY_SECOND_DEVICE_NAME "second_device_name"
#define JSON_KEY_SECOND_DEVICE_PIN  "second_device_pin"

// ===================== IndictmentRecord Fields =====================
#define JSON_KEY_EST_RESISTANCE "est_resistance"
#define JSON_KEY_EST_CAPACITANCE "est_capacitance"
#define JSON_KEY_EST_INDUCTANCE "est_inductance"
#define JSON_KEY_EST_MODEL     "est_model"

#define JSON_KEY_SERIAL_NUMBER "serial_number"

#endif // I3070_CORE_JSONKEYS_HPP 
