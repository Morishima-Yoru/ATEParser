/**
 * @file test_records_shorts.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

using namespace ate;
using namespace ate::core;

TEST(ShortsTestRecord, Parse) {
    auto rec = make_record(enums::Prefix::ts);
    parse_into(rec, {"0", "0", "1", "0", "shorts_block"});
    auto& r = std::get<records::ShortsTestRecord>(rec);
    EXPECT_EQ(r.test_status, enums::GenericTestStatus::pass);
    EXPECT_EQ(r.shorts_count, 0);
    EXPECT_EQ(r.opens_count, 1);
    EXPECT_EQ(r.designator, "shorts_block");
}

TEST(TsSourceRecord, Parse) {
    auto rec = make_record(enums::Prefix::ts_s);
    parse_into(rec, {"2", "1", "NODE_5"});
    auto& r = std::get<records::TsSourceRecord>(rec);
    EXPECT_EQ(r.shorts_count, 2);
    EXPECT_EQ(r.phantoms_count, 1);
    EXPECT_EQ(r.source_node, "NODE_5");
}

TEST(TsDestinationRecord, ParseListPairs) {
    auto rec = make_record(enums::Prefix::ts_d);
    // First field is "\count", then alternating node/deviation
    parse_into(rec, {"\\2", "N1", "0.5", "N2", "1.5"});
    auto& r = std::get<records::TsDestinationRecord>(rec);
    ASSERT_EQ(r.destination_list.size(), 2u);
    EXPECT_EQ(r.destination_list[0].first, "N1");
    EXPECT_DOUBLE_EQ(r.destination_list[0].second, 0.5);
    EXPECT_EQ(r.destination_list[1].first, "N2");
    EXPECT_DOUBLE_EQ(r.destination_list[1].second, 1.5);
}

TEST(TsOpenRecord, Parse) {
    auto rec = make_record(enums::Prefix::ts_o);
    parse_into(rec, {"NODE_A", "NODE_B", "0.123"});
    auto& r = std::get<records::TsOpenRecord>(rec);
    EXPECT_EQ(r.source_node, "NODE_A");
    EXPECT_EQ(r.destination_node, "NODE_B");
    ASSERT_TRUE(r.deviation);
    EXPECT_DOUBLE_EQ(*r.deviation, 0.123);
}

TEST(TsPhantomRecord, ParseDeviation) {
    auto rec = make_record(enums::Prefix::ts_p);
    parse_into(rec, {"0.001"});
    auto& r = std::get<records::TsPhantomRecord>(rec);
    ASSERT_TRUE(r.deviation);
    EXPECT_DOUBLE_EQ(*r.deviation, 0.001);
}
