/**
 * @file test_records_coverage.cpp
 * @brief Coverage tests for record types missing from existing test files.
 *
 * Covers: AlarmBoardRecord, ArrayRecord, NetVerifyRecord, RetestRecord,
 * PfRecord, BoundaryScanRecord, PolarityCheckRecord, ProbeRecord,
 * plus JSON serialization round-trips for all record categories.
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

using namespace ate;
using namespace ate::core;
using json = nlohmann::json;

// ===================================================================
//  Records that had no parse test at all
// ===================================================================

TEST(AlarmBoardRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::aid);
    parse_into(rec, {"2025-07-05T12:00:00", "SN12345"});
    auto& r = std::get<records::AlarmBoardRecord>(rec);
    EXPECT_EQ(r.datetime, "2025-07-05T12:00:00");
    EXPECT_EQ(r.serial, "SN12345");

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@AID");
    EXPECT_EQ(j["datetime"], "2025-07-05T12:00:00");
    EXPECT_EQ(j["serial_number"], "SN12345");
}

TEST(ArrayRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::array);
    parse_into(rec, {"ARR_X", "1", "3", "100"});
    auto& r = std::get<records::ArrayRecord>(rec);
    EXPECT_EQ(r.designator, "ARR_X");
    EXPECT_EQ(r.status, 1);
    ASSERT_TRUE(r.failure_count);
    EXPECT_EQ(*r.failure_count, 3);
    ASSERT_TRUE(r.samples);
    EXPECT_EQ(*r.samples, 100);

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@ARRAY");
    EXPECT_EQ(j["designator"], "ARR_X");
    EXPECT_EQ(j["status"], 1);
    EXPECT_EQ(j["failure_count"], 3);
    EXPECT_EQ(j["samples"], 100);
}

TEST(ArrayRecord, ParseMissingOptionals) {
    auto rec = make_record(enums::Prefix::array);
    parse_into(rec, {"ARR_Y", "0"});
    auto& r = std::get<records::ArrayRecord>(rec);
    EXPECT_EQ(r.designator, "ARR_Y");
    EXPECT_EQ(r.status, 0);
    EXPECT_FALSE(r.failure_count);
    EXPECT_FALSE(r.samples);

    auto j = to_json(rec);
    EXPECT_TRUE(j["failure_count"].is_null());
    EXPECT_TRUE(j["samples"].is_null());
}

TEST(NetVerifyRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::netv);
    parse_into(rec, {"2025-07-05", "SysA", "RepairB", "1"});
    auto& r = std::get<records::NetVerifyRecord>(rec);
    EXPECT_EQ(r.datetime, "2025-07-05");
    EXPECT_EQ(r.test_system, "SysA");
    EXPECT_EQ(r.repair_system, "RepairB");
    EXPECT_TRUE(r.source);

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@NETV");
    EXPECT_EQ(j["test_system"], "SysA");
    EXPECT_EQ(j["repair_system"], "RepairB");
    EXPECT_TRUE(j["source"].get<bool>());
}

TEST(NetVerifyRecord, SourceFalse) {
    auto rec = make_record(enums::Prefix::netv);
    parse_into(rec, {"2025-07-05", "S", "R", "0"});
    auto& r = std::get<records::NetVerifyRecord>(rec);
    EXPECT_FALSE(r.source);
}

TEST(RetestRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::retest);
    parse_into(rec, {"2025-07-05T09:30:00"});
    auto& r = std::get<records::RetestRecord>(rec);
    EXPECT_EQ(r.datetime, "2025-07-05T09:30:00");

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@RETEST");
    EXPECT_EQ(j["datetime"], "2025-07-05T09:30:00");
}

TEST(PfRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::pf);
    parse_into(rec, {"R1", "1", "5"});
    auto& r = std::get<records::PfRecord>(rec);
    EXPECT_EQ(r.designator, "R1");
    ASSERT_TRUE(r.test_status);
    EXPECT_EQ(*r.test_status, 1);
    ASSERT_TRUE(r.total_pins);
    EXPECT_EQ(*r.total_pins, 5);

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@PF");
    EXPECT_EQ(j["designator"], "R1");
    EXPECT_EQ(j["test_status"], 1);
    EXPECT_EQ(j["total_pins"], 5);
}

TEST(PfRecord, ParseMinimal) {
    auto rec = make_record(enums::Prefix::pf);
    parse_into(rec, {"R2"});
    auto& r = std::get<records::PfRecord>(rec);
    EXPECT_EQ(r.designator, "R2");
    EXPECT_FALSE(r.test_status);
    EXPECT_FALSE(r.total_pins);
}

TEST(BoundaryScanRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::bs_con);
    parse_into(rec, {"BS_TEST_1", "0", "2", "3"});
    auto& r = std::get<records::BoundaryScanRecord>(rec);
    EXPECT_EQ(r.test_designator, "BS_TEST_1");
    EXPECT_EQ(r.status, 0);
    ASSERT_TRUE(r.shorts_count);
    EXPECT_EQ(*r.shorts_count, 2);
    ASSERT_TRUE(r.opens_count);
    EXPECT_EQ(*r.opens_count, 3);

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@BS-CON");
    EXPECT_EQ(j["test_designator"], "BS_TEST_1");
    EXPECT_EQ(j["status"], 0);
    EXPECT_EQ(j["shorts_count"], 2);
    EXPECT_EQ(j["opens_count"], 3);
}

TEST(BoundaryScanRecord, ParseMinimalNoOptionals) {
    auto rec = make_record(enums::Prefix::bs_con);
    parse_into(rec, {"BS_TEST_2", "1"});
    auto& r = std::get<records::BoundaryScanRecord>(rec);
    EXPECT_EQ(r.test_designator, "BS_TEST_2");
    EXPECT_EQ(r.status, 1);
    EXPECT_FALSE(r.shorts_count);
    EXPECT_FALSE(r.opens_count);
}

TEST(PolarityCheckRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::pchk);
    parse_into(rec, {"0", "POLARITY_TEST_1"});
    auto& r = std::get<records::PolarityCheckRecord>(rec);
    EXPECT_EQ(r.test_status, enums::GenericTestStatus::pass);
    EXPECT_EQ(r.test_designator, "POLARITY_TEST_1");

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@PCHK");
    EXPECT_EQ(j["status"], 0);
    EXPECT_EQ(j["designator"], "POLARITY_TEST_1");
}

TEST(PolarityCheckRecord, FailedStatus) {
    auto rec = make_record(enums::Prefix::pchk);
    parse_into(rec, {"1", "POL_F"});
    auto& r = std::get<records::PolarityCheckRecord>(rec);
    EXPECT_EQ(r.test_status, enums::GenericTestStatus::fail);
}

TEST(ProbeRecord, ParseAndJson) {
    auto rec = make_record(enums::Prefix::prb);
    parse_into(rec, {"field1", "field2", "field3"});
    auto& r = std::get<records::ProbeRecord>(rec);
    EXPECT_EQ(r.raw_payload, "field1|field2|field3");

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@PRB");
    EXPECT_EQ(j["raw"], "field1|field2|field3");
}

TEST(ProbeRecord, EmptyPayload) {
    auto rec = make_record(enums::Prefix::prb);
    parse_into(rec, {});
    auto& r = std::get<records::ProbeRecord>(rec);
    EXPECT_TRUE(r.raw_payload.empty());
}

// ===================================================================
//  JSON serialization for records that only had parse tests
// ===================================================================

TEST(BatchRecord, JsonAllFields) {
    auto rec = make_record(enums::Prefix::batch);
    parse_into(rec, {"U", "R", "42", "2", "TT", "Step", "B1", "Op",
                     "Ctl", "Plan", "Rev", "PT", "PR", "VL"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@BATCH");
    EXPECT_EQ(j["uut_type"], "U");
    EXPECT_EQ(j["uut_type_rev"], "R");
    EXPECT_EQ(j["fixture_id"], 42);
    EXPECT_EQ(j["testhead_number"], 2);
    EXPECT_EQ(j["testhead_type"], "TT");
    EXPECT_EQ(j["process_step"], "Step");
    EXPECT_EQ(j["batch_id"], "B1");
    EXPECT_EQ(j["operator_id"], "Op");
    EXPECT_EQ(j["controller"], "Ctl");
    EXPECT_EQ(j["testplan_id"], "Plan");
    EXPECT_EQ(j["testplan_rev"], "Rev");
    EXPECT_EQ(j["parent_panel_type"], "PT");
    EXPECT_EQ(j["parent_panel_type_rev"], "PR");
    EXPECT_EQ(j["version_label"], "VL");
}

TEST(BlockRecord, Json) {
    auto rec = make_record(enums::Prefix::block);
    parse_into(rec, {"BLK_A", "1"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@BLOCK");
    EXPECT_EQ(j["block_designator"], "BLK_A");
    EXPECT_EQ(j["block_status"], 1);
}

TEST(AlarmRecord, Json) {
    auto rec = make_record(enums::Prefix::alm);
    parse_into(rec, {"2", "1", "2025-07-05", "BT", "BR", "5", "10", "Ctl", "1"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@ALM");
    EXPECT_EQ(j["alarm_type"], 2);
    EXPECT_TRUE(j["alarm_status"].get<bool>());
    EXPECT_EQ(j["datetime"], "2025-07-05");
    EXPECT_EQ(j["alarm_limit"], 5);
    EXPECT_EQ(j["detected_value"], 10);
}

TEST(AlarmRecord, JsonNullOptionals) {
    auto rec = make_record(enums::Prefix::alm);
    parse_into(rec, {"1", "0", "2025-01-01", "BT", "BR"});
    auto j = to_json(rec);
    EXPECT_TRUE(j["alarm_limit"].is_null());
    EXPECT_TRUE(j["detected_value"].is_null());
}

TEST(BTestRecord, Json) {
    auto rec = make_record(enums::Prefix::btest);
    parse_into(rec, {"BID", "0", "1700000000", "5000", "1", "INFO",
                     "0", "0", "0", "1700000005", "OK", "1", "PanelId"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@BTEST");
    EXPECT_EQ(j["board_id"], "BID");
    EXPECT_EQ(j["test_status"], 0);
    EXPECT_EQ(j["start_datetime"], 1700000000ULL);
    EXPECT_EQ(j["duration"], 5000);
    EXPECT_TRUE(j["multiple_test"].get<bool>());
    EXPECT_EQ(j["log_level"], "INFO");
    EXPECT_EQ(j["log_set"], 0);
    EXPECT_FALSE(j["learning"].get<bool>());
    EXPECT_FALSE(j["known_good"].get<bool>());
    EXPECT_EQ(j["status_qualifier"], "OK");
    EXPECT_EQ(j["board_number"], 1);
    EXPECT_EQ(j["parent_panel_id"], "PanelId");
}

TEST(BTestRecord, JsonNullLogSet) {
    auto rec = make_record(enums::Prefix::btest);
    // Only 6 fields → log_set (field[6]) is absent → optional stays nullopt
    parse_into(rec, {"BID", "0", "1700000000", "5000", "0", "INFO"});
    auto j = to_json(rec);
    EXPECT_TRUE(j["log_set"].is_null());
}

TEST(NodeListRecord, Json) {
    auto rec = make_record(enums::Prefix::node);
    parse_into(rec, {"\\2", "N1", "N2"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@NODE");
    EXPECT_EQ(j["count"], 2);
    EXPECT_EQ(j["nodes"].size(), 2u);
    EXPECT_EQ(j["nodes"][0], "N1");
}

TEST(ReportRecord, Json) {
    auto rec = make_record(enums::Prefix::rpt);
    parse_into(rec, {"hello", "world"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@RPT");
    EXPECT_EQ(j["message"], "hello|world");
}

TEST(ShortsTestRecord, Json) {
    auto rec = make_record(enums::Prefix::ts);
    parse_into(rec, {"1", "2", "3", "1", "shorts_blk"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@TS");
    EXPECT_EQ(j["test_status"], 1);
    EXPECT_EQ(j["shorts_count"], 2);
    EXPECT_EQ(j["opens_count"], 3);
    EXPECT_EQ(j["phantoms_count"], 1);
    EXPECT_EQ(j["designator"], "shorts_blk");
}

TEST(ShortsTestRecord, JsonNullDesignator) {
    auto rec = make_record(enums::Prefix::ts);
    parse_into(rec, {"0", "0", "0", "0"});
    auto j = to_json(rec);
    EXPECT_TRUE(j["designator"].is_null());
}

TEST(TsSourceRecord, Json) {
    auto rec = make_record(enums::Prefix::ts_s);
    parse_into(rec, {"2", "1", "NODE_A"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@TS-S");
    EXPECT_EQ(j["shorts_count"], 2);
    EXPECT_EQ(j["phantoms_count"], 1);
    EXPECT_EQ(j["source_node"], "NODE_A");
}

TEST(TsDestinationRecord, Json) {
    auto rec = make_record(enums::Prefix::ts_d);
    parse_into(rec, {"\\2", "N1", "0.5", "N2", "1.5"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@TS-D");
    ASSERT_EQ(j["destinations_list"].size(), 2u);
    EXPECT_EQ(j["destinations_list"][0][0], "N1");
    EXPECT_DOUBLE_EQ(j["destinations_list"][0][1].get<double>(), 0.5);
}

TEST(TsOpenRecord, Json) {
    auto rec = make_record(enums::Prefix::ts_o);
    parse_into(rec, {"SRC", "DST", "0.42"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@TS-O");
    EXPECT_EQ(j["source_node"], "SRC");
    EXPECT_EQ(j["destination_node"], "DST");
    EXPECT_DOUBLE_EQ(j["deviation"].get<double>(), 0.42);
}

TEST(TsOpenRecord, JsonNullDeviation) {
    auto rec = make_record(enums::Prefix::ts_o);
    parse_into(rec, {"SRC", "DST"});
    auto j = to_json(rec);
    EXPECT_TRUE(j["deviation"].is_null());
}

TEST(TsPhantomRecord, Json) {
    auto rec = make_record(enums::Prefix::ts_p);
    parse_into(rec, {"0.001"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@TS-P");
    EXPECT_DOUBLE_EQ(j["deviation"].get<double>(), 0.001);
}

TEST(TsPhantomRecord, JsonNoDeviation) {
    auto rec = make_record(enums::Prefix::ts_p);
    parse_into(rec, {});
    auto j = to_json(rec);
    EXPECT_TRUE(j["deviation"].is_null());
}

TEST(DigitalTestRecord, Json) {
    auto rec = make_record(enums::Prefix::d_t);
    parse_into(rec, {"1", "5", "10", "16", "DigTest"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@D-T");
    EXPECT_EQ(j["test_status"], 1);
    EXPECT_EQ(j["substatus"], 5);
    EXPECT_EQ(j["failing_vector_number"], 10);
    EXPECT_EQ(j["pin_count"], 16);
    EXPECT_EQ(j["test_designator"], "DigTest");
}

TEST(DigitalTestRecord, NullFailingVector) {
    records::DigitalTestRecord r{};
    r.failing_vector_number = std::nullopt;
    Record rec = r;
    auto j = to_json(rec);
    EXPECT_TRUE(j["failing_vector_number"].is_null());
}

TEST(DevicePinRecord, Json) {
    auto rec = make_record(enums::Prefix::dpin);
    parse_into(rec, {"U1", "PIN_A", "Plain"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@DPIN");
    EXPECT_EQ(j["device_name"], "U1");
    EXPECT_FALSE(j["node_pin_list"].empty());
    EXPECT_FALSE(j.contains("thru_devnode_list"));
}

TEST(DevicePinRecord, JsonWithThruDevnode) {
    auto rec = make_record(enums::Prefix::dpin);
    auto& r = std::get<records::DevicePinRecord>(rec);
    r.device_name = "U1";
    r.thru_devnode_list = std::vector<std::string>{"N1", "D1", "N2", "D2"};
    auto j = to_json(rec);
    ASSERT_TRUE(j.contains("thru_devnode_list"));
    EXPECT_EQ(j["thru_devnode_list"].size(), 4u);
}

TEST(PldProgrammingRecord, Json) {
    auto rec = make_record(enums::Prefix::d_pld);
    parse_into(rec, {"my.svf", "PROGRAM", "0", "OK", "100"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@D-PLD");
    EXPECT_EQ(j["filename"], "my.svf");
    EXPECT_EQ(j["action"], "PROGRAM");
    EXPECT_EQ(j["action_return_code"], 0);
    EXPECT_EQ(j["result_message"], "OK");
    EXPECT_EQ(j["player_program_counter"], 100);
}

TEST(ConnectCheckRecord, Json) {
    auto rec = make_record(enums::Prefix::cchk);
    parse_into(rec, {"0", "8", "U2"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@CCHK");
    EXPECT_EQ(j["status"], 0);
    EXPECT_EQ(j["pin_count"], 8);
    EXPECT_EQ(j["device_designator"], "U2");
}

TEST(TestJetRecord, Json) {
    auto rec = make_record(enums::Prefix::tjet);
    parse_into(rec, {"1", "12", "TJET_X"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@TJET");
    EXPECT_EQ(j["status"], 1);
    EXPECT_EQ(j["pin_count"], 12);
    EXPECT_EQ(j["designator"], "TJET_X");
}

TEST(IndictmentRecord, Json) {
    auto rec = make_record(enums::Prefix::indict);
    parse_into(rec, {"OHM\\2", "R1", "R2", "1.0", "2.0", "3.0", "model"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@INDICT");
    EXPECT_EQ(j["technique"], "OHM");
    EXPECT_EQ(j["device_list"].size(), 2u);
    EXPECT_EQ(j["device_list"][0], "R1");
    EXPECT_DOUBLE_EQ(j["est_resistance"].get<double>(), 1.0);
    EXPECT_DOUBLE_EQ(j["est_capacitance"].get<double>(), 2.0);
    EXPECT_DOUBLE_EQ(j["est_inductance"].get<double>(), 3.0);
    EXPECT_EQ(j["est_model"], "model");
}

TEST(IndictmentRecord, JsonNullEstimates) {
    auto rec = make_record(enums::Prefix::indict);
    parse_into(rec, {"SOAK"});
    auto j = to_json(rec);
    EXPECT_TRUE(j["est_resistance"].is_null());
    EXPECT_TRUE(j["est_capacitance"].is_null());
    EXPECT_TRUE(j["est_inductance"].is_null());
}

TEST(PinRecord, Json) {
    auto rec = make_record(enums::Prefix::pin);
    parse_into(rec, {"\\2", "P1", "P2"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@PIN");
    EXPECT_EQ(j["pin_count"], 2);
    ASSERT_EQ(j["pins"].size(), 2u);
    EXPECT_EQ(j["pins"][0], "P1");
}

TEST(Lim2Record, Json) {
    auto rec = make_record(enums::Prefix::lim2);
    parse_into(rec, {"100.0", "90.0"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@LIM2");
    EXPECT_DOUBLE_EQ(j["high_limit"].get<double>(), 100.0);
    EXPECT_DOUBLE_EQ(j["low_limit"].get<double>(), 90.0);
}

TEST(Lim3Record, Json) {
    auto rec = make_record(enums::Prefix::lim3);
    parse_into(rec, {"100.0", "110.0", "90.0"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@LIM3");
    EXPECT_DOUBLE_EQ(j["nominal_value"].get<double>(), 100.0);
    EXPECT_DOUBLE_EQ(j["high_limit"].get<double>(), 110.0);
    EXPECT_DOUBLE_EQ(j["low_limit"].get<double>(), 90.0);
}

TEST(BsShortRecord, Json) {
    auto rec = make_record(enums::Prefix::bs_s);
    parse_into(rec, {"S\\3", "N1", "N2", "N3"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@BS-S");
    EXPECT_EQ(j["cause"], "S");
    EXPECT_EQ(j["shorts_count"], 3);
    EXPECT_EQ(j["node_list"].size(), 3u);
}

TEST(BsShortRecord, NumericCauseNoBackslash) {
    auto rec = make_record(enums::Prefix::bs_s);
    parse_into(rec, {"5", "N1"});
    auto& r = std::get<records::BsShortRecord>(rec);
    EXPECT_TRUE(r.cause.empty());
    ASSERT_TRUE(r.shorts_count);
    EXPECT_EQ(*r.shorts_count, 5);
    ASSERT_EQ(r.node_list.size(), 1u);
    EXPECT_EQ(r.node_list[0], "N1");
}

TEST(BsOpenRecord, Json) {
    auto rec = make_record(enums::Prefix::bs_o);
    parse_into(rec, {"U1", "1", "U2", "5"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@BS-O");
    EXPECT_EQ(j["first_device_name"], "U1");
    EXPECT_EQ(j["first_device_pin"], "1");
    EXPECT_EQ(j["second_device_name"], "U2");
    EXPECT_EQ(j["second_device_pin"], "5");
}

TEST(BsOpenRecord, JsonNullSecondDevice) {
    auto rec = make_record(enums::Prefix::bs_o);
    parse_into(rec, {"U1", "1"});
    auto j = to_json(rec);
    EXPECT_TRUE(j["second_device_name"].is_null());
    EXPECT_TRUE(j["second_device_pin"].is_null());
}

// ===================================================================
//  Record utilities (make_record, prefix_of, is_unknown)
// ===================================================================

TEST(RecordUtils, MakeRecordUnknownReturnsMonostate) {
    auto rec = make_record(enums::Prefix::unknown);
    EXPECT_TRUE(is_unknown(rec));
}

TEST(RecordUtils, MonostateJsonIsEmptyObject) {
    auto rec = make_record(enums::Prefix::unknown);
    auto j = to_json(rec);
    EXPECT_TRUE(j.is_object());
    EXPECT_TRUE(j.empty());
}

TEST(RecordUtils, ParseIntoMonostateIsNoOp) {
    auto rec = make_record(enums::Prefix::unknown);
    parse_into(rec, {"data", "more"});
    EXPECT_TRUE(is_unknown(rec));
}

TEST(RecordUtils, PrefixOfForAllTypes) {
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::a_res)),  enums::Prefix::a_res);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::d_t)),    enums::Prefix::d_t);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::ts)),     enums::Prefix::ts);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::batch)),  enums::Prefix::batch);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::lim2)),   enums::Prefix::lim2);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::lim3)),   enums::Prefix::lim3);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::pf)),     enums::Prefix::pf);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::prb)),    enums::Prefix::prb);
    EXPECT_EQ(prefix_of(make_record(enums::Prefix::bs_con)), enums::Prefix::bs_con);
}

TEST(RecordUtils, IsUnknownFalseForRealRecord) {
    auto rec = make_record(enums::Prefix::batch);
    EXPECT_FALSE(is_unknown(rec));
}

// ===================================================================
//  Analog helpers not yet covered
// ===================================================================

TEST(AnalogRecord, AbortedStatus) {
    auto rec = make_record(enums::Prefix::a_res);
    parse_into(rec, {"11", "0.0"});
    auto& r = std::get<records::AnalogTestRecord>(rec);
    EXPECT_TRUE(r.aborted());
    EXPECT_TRUE(r.failed());
    EXPECT_FALSE(r.passed());
    EXPECT_EQ(r.status_description(), "Aborted by Operator");
}

TEST(AnalogRecord, UnknownStatusCodeFallsBackToGeneralFail) {
    auto rec = make_record(enums::Prefix::a_res);
    parse_into(rec, {"99", "1.0"});
    auto& r = std::get<records::AnalogTestRecord>(rec);
    EXPECT_EQ(r.test_status, enums::AnalogTestStatus::failed_general);
}

TEST(AnalogRecord, NoMeasuredValue) {
    auto rec = make_record(enums::Prefix::a_res);
    parse_into(rec, {"0"});
    auto& r = std::get<records::AnalogTestRecord>(rec);
    EXPECT_FALSE(r.measured_value.has_value());
    auto j = to_json(rec);
    EXPECT_FALSE(j.contains("measured_value"));
}

TEST(AnalogRecord, IsAnalogTestPrefix) {
    EXPECT_TRUE(records::is_analog_test_prefix(enums::Prefix::a_cap));
    EXPECT_TRUE(records::is_analog_test_prefix(enums::Prefix::a_zen));
    EXPECT_FALSE(records::is_analog_test_prefix(enums::Prefix::d_t));
    EXPECT_FALSE(records::is_analog_test_prefix(enums::Prefix::batch));
}

// ===================================================================
//  Lim3 helpers
// ===================================================================

TEST(Lim3Record, AsymmetricTolerance) {
    records::Lim3Record r{};
    r.nominal_value = 100.0;
    r.high_limit    = 120.0;
    r.low_limit     = 90.0;
    EXPECT_FALSE(r.has_symmetric_tolerance());
    EXPECT_DOUBLE_EQ(r.positive_tolerance(), 20.0);
    EXPECT_DOUBLE_EQ(r.negative_tolerance(), 10.0);
}

TEST(Lim3Record, WithinBoundary) {
    records::Lim3Record r{};
    r.nominal_value = 100.0;
    r.high_limit    = 110.0;
    r.low_limit     = 90.0;
    EXPECT_TRUE(r.within(90.0));
    EXPECT_TRUE(r.within(110.0));
    EXPECT_FALSE(r.within(89.9));
    EXPECT_FALSE(r.within(110.1));
}

TEST(Lim2Record, BoundaryWithin) {
    records::Lim2Record r{};
    r.high_limit = 100.0;
    r.low_limit  = 50.0;
    EXPECT_TRUE(r.within(50.0));
    EXPECT_TRUE(r.within(100.0));
    EXPECT_FALSE(r.within(49.9));
    EXPECT_FALSE(r.within(100.1));
    EXPECT_FALSE(r.has_nominal());
    EXPECT_DOUBLE_EQ(r.range(), 50.0);
    EXPECT_DOUBLE_EQ(r.center(), 75.0);
}

// ===================================================================
//  DigitalTestRecord helpers
// ===================================================================

TEST(DigitalTestRecord, StatusHelpers) {
    records::DigitalTestRecord r{};
    r.test_status = enums::DigitalTestStatus::passed;
    EXPECT_TRUE(r.passed());
    EXPECT_FALSE(r.failed());
    EXPECT_FALSE(r.fatal_error());

    r.test_status = enums::DigitalTestStatus::fatal_error;
    EXPECT_TRUE(r.fatal_error());
    EXPECT_FALSE(r.passed());
}

TEST(DigitalTestRecord, UnknownStatusFallsBack) {
    auto rec = make_record(enums::Prefix::d_t);
    parse_into(rec, {"99", "0"});
    auto& r = std::get<records::DigitalTestRecord>(rec);
    EXPECT_EQ(r.test_status, enums::DigitalTestStatus::failed);
}

TEST(DigitalTestRecord, SubstatusAllZero) {
    records::DigitalTestRecord r{};
    r.test_substatus = 0;
    auto flags = r.substatus_flags();
    EXPECT_TRUE(flags.empty());
}
