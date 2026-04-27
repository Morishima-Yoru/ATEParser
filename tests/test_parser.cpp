/**
 * @file test_parser.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

using namespace ate;
using namespace ate::core;

TEST(Parser, EmptyInputThrowsIntegrity) {
    Parser p;
    EXPECT_THROW(p.parse(""), IntegrityError);
}

TEST(Parser, MissingBatchThrowsIntegrity) {
    Parser p;
    EXPECT_THROW(p.parse("{@A-RES|0|1.0E+03}"), IntegrityError);
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

TEST(Parser, ParseFields_Pin) {
    auto pf = Parser::parse_fields("@PIN\\3|P1|P2|P3");
    EXPECT_EQ(pf.prefix, "@PIN");
    ASSERT_EQ(pf.fields.size(), 4u);
    EXPECT_EQ(pf.fields[0], "\\3");
}
