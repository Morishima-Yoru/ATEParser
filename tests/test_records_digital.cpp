/**
 * @file test_records_digital.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

using namespace ate;
using namespace ate::core;

TEST(DigitalTestRecord, Parse) {
    auto rec = make_record(enums::Prefix::d_t);
    parse_into(rec, {"1", "33", "5", "16", "U1.test"});
    auto& r = std::get<records::DigitalTestRecord>(rec);
    EXPECT_EQ(r.test_status, enums::DigitalTestStatus::failed);
    EXPECT_EQ(r.test_substatus, 33);
    ASSERT_TRUE(r.failing_vector_number);
    EXPECT_EQ(*r.failing_vector_number, 5);
    EXPECT_EQ(r.pin_count, 16);
    EXPECT_EQ(r.test_designator, "U1.test");
}

TEST(DigitalTestRecord, SubstatusFlags) {
    records::DigitalTestRecord r{};
    r.set_substatus_flags({1, 4, 16});
    EXPECT_EQ(r.test_substatus, 21);
    auto flags = r.substatus_flags();
    EXPECT_EQ(flags.size(), 3u);
}

TEST(DevicePinRecord, ParseGroupedNodePins) {
    auto rec = make_record(enums::Prefix::dpin);
    // device_name comes from fields[0] before backslash; here just plain
    parse_into(rec, {"U1", "PIN_A", "Group\\2", "X", "Y"});
    auto& r = std::get<records::DevicePinRecord>(rec);
    EXPECT_EQ(r.device_name, "U1");
    EXPECT_EQ(r.node_pin_list[0], "PIN_A");
    EXPECT_EQ(r.node_pin_list[1], "Group");
    EXPECT_EQ(r.node_pin_list[2][0], "X");
    EXPECT_EQ(r.node_pin_list[2][1], "Y");
}

TEST(DevicePinRecord, ParseGroupedNodePinsTruncated) {
    auto rec = make_record(enums::Prefix::dpin);
    parse_into(rec, {"U1\\7", "PIN_A", "Group\\3", "X", "Y"});
    auto& r = std::get<records::DevicePinRecord>(rec);
    EXPECT_EQ(r.device_name, "U1");
    ASSERT_EQ(r.node_pin_list.size(), 3u);
    EXPECT_EQ(r.node_pin_list[0], "PIN_A");
    EXPECT_EQ(r.node_pin_list[1], "Group");
    ASSERT_TRUE(r.node_pin_list[2].is_array());
    ASSERT_EQ(r.node_pin_list[2].size(), 2u);
    EXPECT_EQ(r.node_pin_list[2][0], "X");
    EXPECT_EQ(r.node_pin_list[2][1], "Y");
}

TEST(DevicePinRecord, ParsePlainMiddleFieldAndTail) {
    auto rec = make_record(enums::Prefix::dpin);
    parse_into(rec, {"U1", "PIN_A", "Plain", "Tail"});
    auto& r = std::get<records::DevicePinRecord>(rec);
    EXPECT_EQ(r.device_name, "U1");
    ASSERT_EQ(r.node_pin_list.size(), 3u);
    EXPECT_EQ(r.node_pin_list[0], "PIN_A");
    EXPECT_EQ(r.node_pin_list[1], "Plain");
    EXPECT_EQ(r.node_pin_list[2], "Tail");
}

TEST(DevicePinRecord, AddDriveThruPair) {
    records::DevicePinRecord r{};
    r.add_drive_thru_pair("N1", "D1");
    ASSERT_TRUE(r.thru_devnode_list.has_value());
    ASSERT_EQ(r.thru_devnode_list->size(), 2u);
    EXPECT_EQ((*r.thru_devnode_list)[0], "N1");
    EXPECT_EQ((*r.thru_devnode_list)[1], "D1");
}

TEST(PldProgrammingRecord, Parse) {
    auto rec = make_record(enums::Prefix::d_pld);
    parse_into(rec, {"my.svf", "PROGRAM", "0", "OK", "100"});
    auto& r = std::get<records::PldProgrammingRecord>(rec);
    EXPECT_EQ(r.filename, "my.svf");
    EXPECT_EQ(r.action, "PROGRAM");
    EXPECT_EQ(r.action_return_code, 0);
    EXPECT_EQ(r.player_program_counter, 100);
}

TEST(ConnectCheckRecord, Parse) {
    auto rec = make_record(enums::Prefix::cchk);
    parse_into(rec, {"0", "8", "U2"});
    auto& r = std::get<records::ConnectCheckRecord>(rec);
    EXPECT_EQ(r.test_status, enums::GenericTestStatus::pass);
    EXPECT_EQ(r.pin_count, 8);
    EXPECT_EQ(r.device_designator, "U2");
}

TEST(IndictmentRecord, ParseTechniqueAndDevices) {
    auto rec = make_record(enums::Prefix::indict);
    parse_into(rec, {"OHM\\2", "R1", "R2", "1.0", "", "", "model"});
    auto& r = std::get<records::IndictmentRecord>(rec);
    EXPECT_EQ(r.technique, "OHM");
    ASSERT_EQ(r.device_list.size(), 2u);
    EXPECT_EQ(r.device_list[0], "R1");
    EXPECT_EQ(r.device_list[1], "R2");
    ASSERT_TRUE(r.est_resistance);
    EXPECT_DOUBLE_EQ(*r.est_resistance, 1.0);
    EXPECT_EQ(r.est_model, "model");
}

TEST(IndictmentRecord, ParseTechniqueWithoutCount) {
    auto rec = make_record(enums::Prefix::indict);
    parse_into(rec, {"SOAK", "1.0", "2.0", "3.0", "model"});
    auto& r = std::get<records::IndictmentRecord>(rec);
    EXPECT_EQ(r.technique, "SOAK");
    EXPECT_TRUE(r.device_list.empty());
    ASSERT_TRUE(r.est_resistance);
    ASSERT_TRUE(r.est_capacitance);
    ASSERT_TRUE(r.est_inductance);
    EXPECT_DOUBLE_EQ(*r.est_resistance, 1.0);
    EXPECT_DOUBLE_EQ(*r.est_capacitance, 2.0);
    EXPECT_DOUBLE_EQ(*r.est_inductance, 3.0);
    EXPECT_EQ(r.est_model, "model");
}

TEST(IndictmentRecord, ParseTruncatedDeviceList) {
    auto rec = make_record(enums::Prefix::indict);
    parse_into(rec, {"OHM\\3", "R1", "R2"});
    auto& r = std::get<records::IndictmentRecord>(rec);
    EXPECT_EQ(r.technique, "OHM");
    ASSERT_EQ(r.device_list.size(), 2u);
    EXPECT_EQ(r.device_list[0], "R1");
    EXPECT_EQ(r.device_list[1], "R2");
    EXPECT_FALSE(r.est_resistance);
    EXPECT_FALSE(r.est_capacitance);
    EXPECT_FALSE(r.est_inductance);
    EXPECT_TRUE(r.est_model.empty());
}

TEST(PinRecord, ParseCountAndPins) {
    auto rec = make_record(enums::Prefix::pin);
    parse_into(rec, {"\\3", "P1", "P2", "P3"});
    auto& r = std::get<records::PinRecord>(rec);
    ASSERT_TRUE(r.pin_count);
    EXPECT_EQ(*r.pin_count, 3);
    ASSERT_EQ(r.pins.size(), 3u);
    EXPECT_EQ(r.pins[0], "P1");
}

TEST(TestJetRecord, Parse) {
    auto rec = make_record(enums::Prefix::tjet);
    parse_into(rec, {"1", "12", "test_x"});
    auto& r = std::get<records::TestJetRecord>(rec);
    EXPECT_EQ(r.test_status, enums::GenericTestStatus::fail);
    EXPECT_EQ(r.pin_count, 12);
    EXPECT_EQ(r.test_designator, "test_x");
}
