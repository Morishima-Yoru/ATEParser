/**
 * @file test_records_system.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

using namespace ate;
using namespace ate::core;

TEST(BatchRecord, Parse) {
    auto rec = make_record(enums::Prefix::batch);
    parse_into(rec, {"BoardX", "RevA", "1234", "1", "Type", "Step1",
                     "Batch001", "OPER01", "CtlA", "PlanX", "RevB",
                     "PanelType", "PanelRev", "VLabel"});
    auto& r = std::get<records::BatchRecord>(rec);
    EXPECT_EQ(r.uut_type, "BoardX");
    EXPECT_EQ(r.fixture_id, 1234);
    EXPECT_EQ(r.testhead_number, 1);
    EXPECT_EQ(r.version_label, "VLabel");
}

TEST(BlockRecord, Parse) {
    auto rec = make_record(enums::Prefix::block);
    parse_into(rec, {"BLK_A", "0"});
    auto& r = std::get<records::BlockRecord>(rec);
    EXPECT_EQ(r.block_designator, "BLK_A");
    EXPECT_EQ(r.block_status, 0);
}

TEST(AlarmRecord, Parse) {
    auto rec = make_record(enums::Prefix::alm);
    parse_into(rec, {"2", "1", "2025-07-05", "BTYPE", "Rev1", "5", "10", "Ctl", "1"});
    auto& r = std::get<records::AlarmRecord>(rec);
    EXPECT_EQ(r.alarm_type, 2);
    EXPECT_TRUE(r.alarm_status);
    ASSERT_TRUE(r.alarm_limit);
    EXPECT_EQ(*r.alarm_limit, 5);
    ASSERT_TRUE(r.detected_value);
    EXPECT_EQ(*r.detected_value, 10);
}

TEST(NodeListRecord, ParseCountAndNodes) {
    auto rec = make_record(enums::Prefix::node);
    parse_into(rec, {"\\2", "N1", "N2"});
    auto& r = std::get<records::NodeListRecord>(rec);
    EXPECT_EQ(r.count, 2);
    ASSERT_EQ(r.nodes.size(), 2u);
    EXPECT_EQ(r.nodes[0], "N1");
}

TEST(NodeListRecord, SingleNodeNoBackslash) {
    auto rec = make_record(enums::Prefix::node);
    parse_into(rec, {"OnlyNode"});
    auto& r = std::get<records::NodeListRecord>(rec);
    EXPECT_EQ(r.count, 1);
    ASSERT_EQ(r.nodes.size(), 1u);
    EXPECT_EQ(r.nodes[0], "OnlyNode");
}

TEST(ReportRecord, JoinsFieldsWithPipe) {
    auto rec = make_record(enums::Prefix::rpt);
    parse_into(rec, {"hello", "world", "again"});
    auto& r = std::get<records::ReportRecord>(rec);
    EXPECT_EQ(r.message, "hello|world|again");
}

TEST(BsShortRecord, ParseCauseAndCount) {
    auto rec = make_record(enums::Prefix::bs_s);
    parse_into(rec, {"S\\3", "N1", "N2", "N3"});
    auto& r = std::get<records::BsShortRecord>(rec);
    EXPECT_EQ(r.cause, "S");
    ASSERT_TRUE(r.shorts_count);
    EXPECT_EQ(*r.shorts_count, 3);
    ASSERT_EQ(r.node_list.size(), 3u);
}

TEST(BsOpenRecord, Parse) {
    auto rec = make_record(enums::Prefix::bs_o);
    parse_into(rec, {"U1", "1", "U2", "5"});
    auto& r = std::get<records::BsOpenRecord>(rec);
    EXPECT_EQ(r.first_device_name, "U1");
    EXPECT_EQ(r.first_device_pin, "1");
    EXPECT_EQ(r.second_device_name, "U2");
}

TEST(BTestRecord, Parse) {
    auto rec = make_record(enums::Prefix::btest);
    parse_into(rec, {"BoardId", "0", "1700000000", "5000", "1", "INFO",
                     "0", "0", "0", "1700000005", "OK", "1", "PanelId"});
    auto& r = std::get<records::BTestRecord>(rec);
    EXPECT_EQ(r.board_id, "BoardId");
    EXPECT_EQ(r.test_status, 0);
    EXPECT_EQ(r.start_datetime, 1700000000ULL);
    EXPECT_EQ(r.duration, 5000);
    EXPECT_TRUE(r.multiple_test);
    EXPECT_EQ(r.parent_panel_id, "PanelId");
}
