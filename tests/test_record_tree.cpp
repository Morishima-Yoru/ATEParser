/**
 * @file test_record_tree.cpp
 * @brief Tests for RecordNode serialization and C ABI entry points.
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>

using namespace ate;
using namespace ate::core;
using json = nlohmann::json;

// ===================================================================
//  RecordNode to_json
// ===================================================================

TEST(RecordTree, EmptyNodeIsEmptyObject) {
    RecordNode node{};
    auto j = to_json(node);
    EXPECT_TRUE(j.is_object());
    EXPECT_TRUE(j.empty());
}

TEST(RecordTree, SingleRecordNoChildren) {
    RecordNode node{};
    node.record = make_record(enums::Prefix::batch);
    parse_into(node.record, {"U", "R", "1", "1", "TT", "S", "B", "O", "C", "P", "R", "PT", "PR", "VL"});
    auto j = to_json(node);
    EXPECT_EQ(j["prefix"], "@BATCH");
    EXPECT_EQ(j["uut_type"], "U");
    EXPECT_FALSE(j.contains("sub"));
}

TEST(RecordTree, WithChildren) {
    RecordNode root{};
    root.record = make_record(enums::Prefix::batch);
    parse_into(root.record, {"U", "R", "0", "1", "TT", "S", "B", "O", "C", "P", "R", "PT", "PR", "VL"});

    RecordNode child{};
    child.record = make_record(enums::Prefix::a_res);
    parse_into(child.record, {"0", "100.5"});
    root.children.push_back(std::move(child));

    auto j = to_json(root);
    ASSERT_TRUE(j.contains("sub"));
    ASSERT_EQ(j["sub"].size(), 1u);
    EXPECT_EQ(j["sub"][0]["prefix"], "@A-RES");
}

TEST(RecordTree, KeepRawTrue) {
    RecordNode node{};
    node.record = make_record(enums::Prefix::block);
    parse_into(node.record, {"BLK_A", "0"});
    node.raw_data = "BLK_A|0";

    auto j_no_raw = to_json(node, false);
    EXPECT_FALSE(j_no_raw.contains("raw"));

    auto j_raw = to_json(node, true);
    ASSERT_TRUE(j_raw.contains("raw"));
    EXPECT_EQ(j_raw["raw"], "BLK_A|0");
}

TEST(RecordTree, KeepRawEmptyStringSkipped) {
    RecordNode node{};
    node.record = make_record(enums::Prefix::block);
    parse_into(node.record, {"X", "1"});
    node.raw_data = "";

    auto j = to_json(node, true);
    EXPECT_FALSE(j.contains("raw"));
}

TEST(RecordTree, UnknownRecordNodeEmitsEmptyObject) {
    RecordNode node{};
    node.record = std::monostate{};
    node.raw_data = "UNKNOWN_DATA";

    auto j = to_json(node, true);
    EXPECT_TRUE(j.is_object());
    // The only key should be "raw" since keep_raw is true and raw_data is non-empty.
    EXPECT_TRUE(j.contains("raw"));
    EXPECT_EQ(j["raw"], "UNKNOWN_DATA");
}

TEST(RecordTree, NestedChildren) {
    RecordNode root{};
    root.record = make_record(enums::Prefix::batch);
    parse_into(root.record, {"U", "R", "0", "1", "TT", "S", "B", "O", "C", "P", "R", "PT", "PR", "VL"});

    RecordNode bt{};
    bt.record = make_record(enums::Prefix::btest);
    parse_into(bt.record, {"BID", "0", "170", "10", "0", "INFO"});

    RecordNode analog{};
    analog.record = make_record(enums::Prefix::a_cap);
    parse_into(analog.record, {"0", "1.5E-06"});
    bt.children.push_back(std::move(analog));

    root.children.push_back(std::move(bt));

    auto j = to_json(root);
    ASSERT_TRUE(j.contains("sub"));
    ASSERT_EQ(j["sub"].size(), 1u);
    ASSERT_TRUE(j["sub"][0].contains("sub"));
    EXPECT_EQ(j["sub"][0]["sub"][0]["prefix"], "@A-CAP");
}

// ===================================================================
//  C ABI: ate_parse_log_string
// ===================================================================

TEST(CABI, ParseLogString_Success) {
    const char* log = "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}";
    char* out = nullptr;
    int rc = ate_parse_log_string(log, nullptr, -1, 0, &out);
    ASSERT_EQ(rc, static_cast<int>(ParseResult::success));
    ASSERT_NE(out, nullptr);

    auto j = json::parse(out);
    EXPECT_EQ(j["prefix"], "@BATCH");
    EXPECT_EQ(j["uut_type"], "U");

    ate_free_string(out);
}

TEST(CABI, ParseLogString_WithIndent) {
    const char* log = "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}";
    char* out = nullptr;
    int rc = ate_parse_log_string(log, nullptr, 2, 0, &out);
    ASSERT_EQ(rc, static_cast<int>(ParseResult::success));
    ASSERT_NE(out, nullptr);

    std::string s(out);
    EXPECT_NE(s.find('\n'), std::string::npos); // indented JSON contains newlines
    ate_free_string(out);
}

TEST(CABI, ParseLogString_KeepRaw) {
    const char* log = "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL"
                          "{@A-RES|0|100.0}"
                      "}";
    char* out = nullptr;
    int rc = ate_parse_log_string(log, nullptr, -1, 1, &out);
    ASSERT_EQ(rc, static_cast<int>(ParseResult::success));
    ASSERT_NE(out, nullptr);

    auto j = json::parse(out);
    ASSERT_TRUE(j.contains("raw"));
    ate_free_string(out);
}

TEST(CABI, ParseLogString_NullInput) {
    char* out = nullptr;
    int rc = ate_parse_log_string(nullptr, nullptr, -1, 0, &out);
    EXPECT_EQ(rc, static_cast<int>(ParseResult::input_error));
}

TEST(CABI, ParseLogString_NullOutPtr) {
    const char* log = "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}";
    int rc = ate_parse_log_string(log, nullptr, -1, 0, nullptr);
    EXPECT_EQ(rc, static_cast<int>(ParseResult::input_error));
}

TEST(CABI, ParseLogString_IntegrityError) {
    // No @BATCH record -> integrity error
    char* out = nullptr;
    int rc = ate_parse_log_string("{@A-RES|0|100}", nullptr, -1, 0, &out);
    EXPECT_EQ(rc, static_cast<int>(ParseResult::failed_integrity));
}

TEST(CABI, FreeStringNull) {
    // Should not crash
    ate_free_string(nullptr);
}

// ===================================================================
//  Parser: keep_raw mode
// ===================================================================

TEST(Parser, KeepRawMode) {
    Parser p;
    auto root = p.parse("{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL"
                             "{@A-RES|0|50.0}"
                         "}", true);
    auto& batch = root.children[0];
    EXPECT_FALSE(batch.raw_data.empty());
    auto j = to_json(batch, true);
    ASSERT_TRUE(j.contains("raw"));
}

TEST(Parser, KeepRawFalseByDefault) {
    Parser p;
    auto root = p.parse("{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}");
    auto& batch = root.children[0];
    EXPECT_TRUE(batch.raw_data.empty());
}

// ===================================================================
//  Parser::parse_fields
// ===================================================================

TEST(Parser, ParseFields_Empty) {
    auto pf = Parser::parse_fields("@BATCH");
    EXPECT_EQ(pf.prefix, "@BATCH");
    EXPECT_TRUE(pf.fields.empty());
}

TEST(Parser, ParseFields_SingleField) {
    auto pf = Parser::parse_fields("@BLOCK|BLK_A");
    EXPECT_EQ(pf.prefix, "@BLOCK");
    ASSERT_EQ(pf.fields.size(), 1u);
    EXPECT_EQ(pf.fields[0], "BLK_A");
}

TEST(Parser, ParseFields_MultipleFields) {
    auto pf = Parser::parse_fields("@A-RES|0|100.5|sub_test");
    EXPECT_EQ(pf.prefix, "@A-RES");
    ASSERT_EQ(pf.fields.size(), 3u);
    EXPECT_EQ(pf.fields[0], "0");
    EXPECT_EQ(pf.fields[1], "100.5");
    EXPECT_EQ(pf.fields[2], "sub_test");
}

// ===================================================================
//  Parser edge cases
// ===================================================================

TEST(Parser, DeepNesting) {
    Parser p;
    const std::string log =
        "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL"
            "{@BTEST|BID|0|170|10|0|INFO"
                "{@BLOCK|BLK1|0"
                    "{@A-RES|0|100.0}"
                "}"
            "}"
        "}";
    auto root = p.parse(log);
    auto& batch = root.children[0];
    auto& btest = batch.children[0];
    auto& block = btest.children[0];
    EXPECT_EQ(prefix_of(block.record), enums::Prefix::block);
    ASSERT_EQ(block.children.size(), 1u);
    EXPECT_EQ(prefix_of(block.children[0].record), enums::Prefix::a_res);
}

TEST(Parser, MultipleTopLevelChildrenUnderBtest) {
    Parser p;
    const std::string log =
        "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL"
            "{@BTEST|BID|0|170|10|0|INFO"
                "{@A-RES|0|100.0}"
                "{@A-CAP|0|2.0E-06}"
                "{@D-T|0|0|0|4|DIG_TEST}"
            "}"
        "}";
    auto root = p.parse(log);
    auto& btest = root.children[0].children[0];
    EXPECT_EQ(btest.children.size(), 3u);
}

// ===================================================================
//  C ABI: ate_parse_file
// ===================================================================

TEST(CABI, ParseFile_Success) {
    auto tmp = std::filesystem::temp_directory_path() / "test_ate_parse_file_input.txt";
    std::error_code ec_rm;
    std::filesystem::remove(tmp, ec_rm);
    {
        std::ofstream ofs(tmp);
        ofs << "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}";
    }

    char* out = nullptr;
    int rc = ate_parse_file(tmp.string().c_str(), nullptr, -1, 0, &out);
    ASSERT_EQ(rc, static_cast<int>(ParseResult::success));
    ASSERT_NE(out, nullptr);

    auto j = json::parse(out);
    EXPECT_EQ(j["prefix"], "@BATCH");
    ate_free_string(out);
    std::filesystem::remove(tmp, ec_rm);
}

TEST(CABI, ParseFile_NullFilepath) {
    char* out = nullptr;
    int rc = ate_parse_file(nullptr, nullptr, -1, 0, &out);
    EXPECT_EQ(rc, static_cast<int>(ParseResult::input_error));
}

TEST(CABI, ParseFile_NullOutPtr) {
    int rc = ate_parse_file("some_path", nullptr, -1, 0, nullptr);
    EXPECT_EQ(rc, static_cast<int>(ParseResult::input_error));
}

TEST(CABI, ParseFile_NonExistentFile) {
    char* out = nullptr;
    int rc = ate_parse_file("__nonexistent_file__.txt", nullptr, -1, 0, &out);
    EXPECT_EQ(rc, static_cast<int>(ParseResult::input_error));
}

TEST(CABI, ParseFile_EmptyFile) {
    auto tmp = std::filesystem::temp_directory_path() / "test_ate_parse_file_empty.txt";
    std::error_code ec_rm;
    std::filesystem::remove(tmp, ec_rm);
    { std::ofstream ofs(tmp); }
    char* out = nullptr;
    int rc = ate_parse_file(tmp.string().c_str(), nullptr, -1, 0, &out);
    EXPECT_EQ(rc, static_cast<int>(ParseResult::input_error));
    std::filesystem::remove(tmp, ec_rm);
}

TEST(CABI, ParseLogString_WriteToDstFile) {
    auto tmp = std::filesystem::temp_directory_path() / "test_ate_dst_output.json";
    std::error_code ec_rm;
    std::filesystem::remove(tmp, ec_rm);

    const char* log = "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}";
    char* out = nullptr;
    int rc = ate_parse_log_string(log, tmp.string().c_str(), 2, 0, &out);
    ASSERT_EQ(rc, static_cast<int>(ParseResult::success));
    ASSERT_NE(out, nullptr);

    {
        std::ifstream ifs(tmp);
        ASSERT_TRUE(ifs.good());
        std::string content((std::istreambuf_iterator<char>(ifs)),
                             std::istreambuf_iterator<char>());
        EXPECT_FALSE(content.empty());
        auto j = json::parse(content);
        EXPECT_EQ(j["prefix"], "@BATCH");
    }

    ate_free_string(out);
    std::filesystem::remove(tmp, ec_rm);
}

TEST(CABI, ParseFile_WriteToDstFile) {
    auto src = std::filesystem::temp_directory_path() / "test_ate_pf_src.txt";
    auto dst = std::filesystem::temp_directory_path() / "test_ate_pf_dst.json";
    std::error_code ec_rm;
    std::filesystem::remove(src, ec_rm);
    std::filesystem::remove(dst, ec_rm);

    {
        std::ofstream ofs(src);
        ofs << "{@BATCH|U|R|0|1|TT|S|B|O|C|P|R|PT|PR|VL}";
    }

    char* out = nullptr;
    int rc = ate_parse_file(src.string().c_str(), dst.string().c_str(), 2, 0, &out);
    ASSERT_EQ(rc, static_cast<int>(ParseResult::success));
    ASSERT_NE(out, nullptr);

    {
        std::ifstream ifs(dst);
        ASSERT_TRUE(ifs.good());
        std::string content((std::istreambuf_iterator<char>(ifs)),
                             std::istreambuf_iterator<char>());
        auto j = json::parse(content);
        EXPECT_EQ(j["prefix"], "@BATCH");
    }

    ate_free_string(out);
    std::filesystem::remove(src, ec_rm);
    std::filesystem::remove(dst, ec_rm);
}
