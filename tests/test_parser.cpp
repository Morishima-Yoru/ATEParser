/**
 * @file test_parser.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

using namespace ate;
using namespace ate::core;

TEST(Parser, EmptyInputThrowsIntegrity) {
    Parser p;
    EXPECT_THROW({ (void)p.parse(""); }, IntegrityError);
}

TEST(Parser, MissingBatchThrowsIntegrity) {
    Parser p;
    EXPECT_THROW({ (void)p.parse("{@A-RES|0|1.0E+03}"); }, IntegrityError);
}

TEST(Parser, MinimalBatch) {
    Parser p;
    auto root = p.parse("{@BATCH|BoardX|RevA|0|1|TT|Step|Batch|Op|Ctl|Plan|R|PT|PR|VL}");
    ASSERT_EQ(root.children.size(), 1u);
    auto& batch = root.children[0];
    EXPECT_EQ(prefix_of(batch.record), enums::Prefix::batch);
    auto& b = std::get<records::BatchRecord>(batch.record);
    EXPECT_EQ(b.uut_type, "BoardX");
    EXPECT_EQ(b.version_label, "VL");
}

TEST(Parser, NestedSubrecords) {
    Parser p;
    const std::string log =
        "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL"
            "{@BTEST|BID|0|1700000000|10|0|INFO|0|0|0|1700000010|OK|1|PID"
                "{@A-RES|0|100.0}"
                "{@A-CAP|1|1.5E-06|sub}"
            "}"
        "}";
    auto root = p.parse(log);
    ASSERT_EQ(root.children.size(), 1u);
    auto& batch = root.children[0];
    ASSERT_EQ(batch.children.size(), 1u);
    auto& bt = batch.children[0];
    ASSERT_EQ(bt.children.size(), 2u);
    EXPECT_EQ(prefix_of(bt.children[0].record), enums::Prefix::a_res);
    EXPECT_EQ(prefix_of(bt.children[1].record), enums::Prefix::a_cap);
}

TEST(Parser, JsonShape) {
    Parser p;
    auto root = p.parse("{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}");
    auto j = to_json(root.children.front());
    EXPECT_EQ(j["prefix"], "@BATCH");
    EXPECT_EQ(j["uut_type"], "U");
}

TEST(Parser, ParseFields_Generic) {
    auto pf = Parser::parse_fields("@BLOCK|BLK_A|0");
    EXPECT_EQ(pf.prefix, "@BLOCK");
    ASSERT_EQ(pf.fields.size(), 2u);
    EXPECT_EQ(pf.fields[0], "BLK_A");
    EXPECT_EQ(pf.fields[1], "0");
}

TEST(Parser, ParseFields_AnalogOptionalSubtest) {
    auto pf = Parser::parse_fields("@A-RES|0|1.0E+03");
    EXPECT_EQ(pf.prefix, "@A-RES");
    ASSERT_EQ(pf.fields.size(), 3u);
    EXPECT_EQ(pf.fields[0], "0");
    EXPECT_EQ(pf.fields[1], "1.0E+03");
    EXPECT_TRUE(pf.fields[2].empty());
}

TEST(Parser, ParseFields_RptPlain) {
    auto pf = Parser::parse_fields("@RPT|A|B");
    EXPECT_EQ(pf.prefix, "@RPT");
    ASSERT_EQ(pf.fields.size(), 2u);
    EXPECT_EQ(pf.fields[0], "A");
    EXPECT_EQ(pf.fields[1], "B");
}

TEST(Parser, ParseFields_RptLiteralLength) {
    auto pf = Parser::parse_fields("@RPT~5|HELLO|EXTRA|FIELDS");
    EXPECT_EQ(pf.prefix, "@RPT");
    ASSERT_EQ(pf.fields.size(), 4u);
    EXPECT_EQ(pf.fields[0], "HELLO");
    EXPECT_TRUE(pf.fields[1].empty());
    EXPECT_EQ(pf.fields[2], "EXTRA");
    EXPECT_EQ(pf.fields[3], "FIELDS");
}

TEST(Parser, ParseFields_RptMalformedLength) {
    EXPECT_THROW({ (void)Parser::parse_fields("@RPT~X|HELLO"); }, MalformedRecordError);
}

TEST(Parser, ParseFields_RptMissingPipeAfterLength) {
    EXPECT_THROW({ (void)Parser::parse_fields("@RPT~5HELLO"); }, MalformedRecordError);
}

TEST(Parser, ParseFields_Pin) {
    auto pf = Parser::parse_fields("@PIN\\3|P1|P2|P3");
    EXPECT_EQ(pf.prefix, "@PIN");
    ASSERT_EQ(pf.fields.size(), 4u);
    EXPECT_EQ(pf.fields[0], "\\3");
}

TEST(Parser, ParseFields_PinNoCountSyntax) {
    auto pf = Parser::parse_fields("@PIN|P1|P2");
    EXPECT_EQ(pf.prefix, "@PIN");
    ASSERT_EQ(pf.fields.size(), 2u);
    EXPECT_EQ(pf.fields[0], "P1");
    EXPECT_EQ(pf.fields[1], "P2");
}

TEST(Parser, ParseFields_PinMissingPipeAfterCount) {
    EXPECT_THROW({ (void)Parser::parse_fields("@PIN\\3NoPipe"); }, MalformedRecordError);
}

TEST(Parser, ParseFields_TsdWithPipe) {
    auto pf = Parser::parse_fields("@TS-D|A|B");
    EXPECT_EQ(pf.prefix, "@TS-D");
    ASSERT_EQ(pf.fields.size(), 2u);
    EXPECT_EQ(pf.fields[0], "A");
    EXPECT_EQ(pf.fields[1], "B");
}

TEST(Parser, ParseFields_TsdMissingPipeAfterCount) {
    EXPECT_THROW({ (void)Parser::parse_fields("@TS-D\\0"); }, MalformedRecordError);
}

TEST(Parser, ParseUnbalancedBracesThrowsMalformed) {
    Parser p;
    EXPECT_THROW({ (void)p.parse("{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL{@BTEST|BID|0|1700000000|10|0|INFO|0|0|0|1700000010|OK|1|PID"); }, MalformedRecordError);
}

TEST(Parser, FirstRecordMustBeExactBatch) {
    Parser p;
    EXPECT_THROW({ (void)p.parse("{@BATCHX|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}"); }, IntegrityError);
}

TEST(Parser, ParseFields_RPT_LiteralExceedsSize) {
    EXPECT_THROW(
        Parser::parse_fields("@RPT~999|short"),
        MalformedRecordError
    );
}

TEST(Parser, ParseFields_TSD_NoBackslash_WithPipe) {
    auto pf = Parser::parse_fields("@TS-D|0|somedata|extra");
    EXPECT_EQ(pf.prefix, "@TS-D");
    ASSERT_GE(pf.fields.size(), 2u);
    EXPECT_EQ(pf.fields[0], "0");
}

TEST(Parser, ParseFields_TSD_NoBackslash_NoPipe) {
    auto pf = Parser::parse_fields("@TS-D");
    EXPECT_EQ(pf.prefix, "@TS-D");
    EXPECT_TRUE(pf.fields.empty());
}

TEST(Parser, ParseFields_Analog_SinglePipe) {
    auto pf = Parser::parse_fields("@A-RES|0");
    EXPECT_EQ(pf.prefix, "@A-RES");
    ASSERT_EQ(pf.fields.size(), 3u);
    EXPECT_EQ(pf.fields[0], "0");
}

TEST(Parser, ParseFields_Analog_NoPipe) {
    auto pf = Parser::parse_fields("@A-RES");
    EXPECT_EQ(pf.prefix, "@A-RES");
    EXPECT_TRUE(pf.fields.empty());
}

TEST(Parser, EmptyPrefixChildSkipped) {
    Parser p;
    const std::string log =
        "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL"
            "{ }"
            "{@A-RES|0|100.0}"
        "}";
    auto root = p.parse(log);
    auto& batch = root.children[0];
    ASSERT_EQ(batch.children.size(), 1u);
    EXPECT_EQ(prefix_of(batch.children[0].record), enums::Prefix::a_res);
}

TEST(Parser, ParseFields_Analog_ThreeFields) {
    auto pf = Parser::parse_fields("@A-RES|0|100.5|sub_test_1");
    EXPECT_EQ(pf.prefix, "@A-RES");
    ASSERT_EQ(pf.fields.size(), 3u);
    EXPECT_EQ(pf.fields[0], "0");
    EXPECT_EQ(pf.fields[1], "100.5");
    EXPECT_EQ(pf.fields[2], "sub_test_1");
}

TEST(Parser, ParseFields_Analog_TwoFieldsWithBrace) {
    auto pf = Parser::parse_fields("@A-RES|0|100.5{@LIM3|lo|hi|nom}");
    EXPECT_EQ(pf.prefix, "@A-RES");
    ASSERT_EQ(pf.fields.size(), 3u);
    EXPECT_EQ(pf.fields[0], "0");
    EXPECT_EQ(pf.fields[1], "100.5");
}

TEST(Parser, FullParse_AnalogWithSubtest) {
    Parser p;
    const std::string log =
        "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL"
            "{@A-RES|0|55.5|sub_x}"
        "}";
    auto root = p.parse(log);
    auto& batch = root.children[0];
    ASSERT_EQ(batch.children.size(), 1u);
    auto& analog = std::get<records::AnalogTestRecord>(batch.children[0].record);
    EXPECT_EQ(analog.test_status, enums::AnalogTestStatus::passed);
    ASSERT_TRUE(analog.measured_value.has_value());
    EXPECT_DOUBLE_EQ(*analog.measured_value, 55.5);
    ASSERT_TRUE(analog.subtest_designator.has_value());
    EXPECT_EQ(*analog.subtest_designator, "sub_x");
}
