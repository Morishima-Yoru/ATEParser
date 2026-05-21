/**
 * @file test_records_analog.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

using namespace ate;
using namespace ate::core;

TEST(AnalogRecord, ParseResistor) {
    auto rec = make_record(enums::Prefix::a_res);
    parse_into(rec, {"0", "1.0E+03"});
    auto& r = std::get<records::AnalogTestRecord>(rec);
    EXPECT_EQ(r.prefix, enums::Prefix::a_res);
    EXPECT_TRUE(r.passed());
    ASSERT_TRUE(r.measured_value);
    EXPECT_DOUBLE_EQ(*r.measured_value, 1000.0);
    EXPECT_FALSE(r.subtest_designator);
}

TEST(AnalogRecord, ParseCapacitorWithSubtest) {
    auto rec = make_record(enums::Prefix::a_cap);
    parse_into(rec, {"1", "1.5E-06", "secondary"});
    auto& r = std::get<records::AnalogTestRecord>(rec);
    EXPECT_TRUE(r.failed());
    ASSERT_TRUE(r.subtest_designator);
    EXPECT_EQ(*r.subtest_designator, "secondary");
}

TEST(AnalogRecord, JsonShape) {
    auto rec = make_record(enums::Prefix::a_dio);
    parse_into(rec, {"0", "0.7"});
    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@A-DIO");
    EXPECT_EQ(j["test_status"], 0);
    EXPECT_DOUBLE_EQ(j["measured_value"].get<double>(), 0.7);
    EXPECT_FALSE(j.contains("subtest_designator"));
}

TEST(AnalogRecord, AllAnalogPrefixesMakeAnalogTest) {
    using P = enums::Prefix;
    for (auto p : {P::a_cap, P::a_dio, P::a_fus, P::a_ind, P::a_jum, P::a_mea,
                   P::a_nfe, P::a_npn, P::a_pfe, P::a_pnp, P::a_pot, P::a_res,
                   P::a_swi, P::a_zen}) {
        auto rec = make_record(p);
        ASSERT_TRUE(std::holds_alternative<records::AnalogTestRecord>(rec));
        EXPECT_EQ(std::get<records::AnalogTestRecord>(rec).prefix, p);
    }
}

TEST(Lim2Record, ParseAndJson) {
    auto rec = make_record(enums::Prefix::lim2);
    parse_into(rec, {"100.0", "90.0"});
    auto& r = std::get<records::Lim2Record>(rec);
    EXPECT_DOUBLE_EQ(r.high_limit, 100.0);
    EXPECT_DOUBLE_EQ(r.low_limit, 90.0);
    EXPECT_TRUE(r.within(95.0));
    EXPECT_FALSE(r.within(101.0));
    EXPECT_FALSE(r.has_nominal());
    EXPECT_DOUBLE_EQ(r.range(), 10.0);
    EXPECT_DOUBLE_EQ(r.center(), 95.0);
}

TEST(Lim3Record, ParseAndHelpers) {
    records::Lim3Record r{};
    r.nominal_value = 100.0;
    r.high_limit    = 110.0;
    r.low_limit     = 90.0;
    EXPECT_TRUE(r.has_symmetric_tolerance());
    EXPECT_DOUBLE_EQ(r.percent_deviation(105.0), 5.0);
    EXPECT_TRUE(r.within(95.0));
}

TEST(Lim3Record, PercentDeviationZeroNominalThrows) {
    records::Lim3Record r{};
    EXPECT_THROW({ (void)r.percent_deviation(1.0); }, std::domain_error);
}

TEST(Analog, ExpectsLim3) {
    EXPECT_TRUE(records::expects_lim3(enums::Prefix::a_cap));
    EXPECT_FALSE(records::expects_lim3(enums::Prefix::a_jum));
    EXPECT_THROW({ (void)records::expects_lim3(enums::Prefix::batch); }, std::invalid_argument);
}

TEST(AnalogTestRecord, JsonWithSubtestDesignator) {
    auto rec = make_record(enums::Prefix::a_res);
    auto& r = std::get<records::AnalogTestRecord>(rec);
    r.test_status = enums::AnalogTestStatus::passed;
    r.measured_value = 100.5;
    r.subtest_designator = "SUB_A";

    auto j = to_json(rec);
    EXPECT_EQ(j["prefix"], "@A-RES");
    EXPECT_EQ(j["test_status"], 0);
    EXPECT_DOUBLE_EQ(j["measured_value"].get<double>(), 100.5);
    EXPECT_EQ(j["subtest_designator"], "SUB_A");
}

TEST(AnalogTestRecord, JsonWithEmptySubtestDesignator) {
    auto rec = make_record(enums::Prefix::a_res);
    auto& r = std::get<records::AnalogTestRecord>(rec);
    r.test_status = enums::AnalogTestStatus::passed;
    r.measured_value = 50.0;
    r.subtest_designator = "";

    auto j = to_json(rec);
    EXPECT_FALSE(j.contains("subtest_designator"));
}

TEST(AnalogTestRecord, JsonWithoutOptionals) {
    auto rec = make_record(enums::Prefix::a_res);
    auto& r = std::get<records::AnalogTestRecord>(rec);
    r.test_status = enums::AnalogTestStatus::passed;

    auto j = to_json(rec);
    EXPECT_FALSE(j.contains("measured_value"));
    EXPECT_FALSE(j.contains("subtest_designator"));
}
