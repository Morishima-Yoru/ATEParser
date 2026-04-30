/**
 * @file test_extended_coverage.cpp
 * @brief Extended coverage for safe_conversion, field_value, and error types.
 */
#include <gtest/gtest.h>
#include "ate_parser/utils/safe_conversion.hpp"
#include "ate_parser/utils/errors.hpp"
#include "ate_parser/enums/field_type.hpp"

using namespace ate::utils;
using namespace ate::enums;

// ===================================================================
//  safe_conversion: additional type coverage
// ===================================================================

TEST(SafeConversion, ParseFloat_Ok) {
    auto v = parse_to<float>("2.5");
    ASSERT_TRUE(v);
    EXPECT_FLOAT_EQ(*v, 2.5f);
}

TEST(SafeConversion, ParseFloat_Scientific) {
    auto v = parse_to<float>("+1.5E+02");
    ASSERT_TRUE(v);
    EXPECT_FLOAT_EQ(*v, 150.0f);
}

TEST(SafeConversion, ParseFloat_Invalid) {
    auto v = parse_to<float>("xyz");
    ASSERT_FALSE(v);
    EXPECT_EQ(v.error().errc, ConversionErrc::invalid_syntax);
}

TEST(SafeConversion, ParseLong_Ok) {
    auto v = parse_to<long>("1234567890");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 1234567890L);
}

TEST(SafeConversion, ParseLong_Negative) {
    auto v = parse_to<long>("-999");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, -999L);
}

TEST(SafeConversion, ParseLongLong_Ok) {
    auto v = parse_to<long long>("9223372036854775807");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 9223372036854775807LL);
}

TEST(SafeConversion, ParseLongLong_LeadingPlus) {
    auto v = parse_to<long long>("+100");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 100LL);
}

TEST(SafeConversion, ParseUnsigned_Ok) {
    auto v = parse_to<unsigned>("4294967295");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 4294967295u);
}

TEST(SafeConversion, ParseUnsigned_Negative) {
    auto v = parse_to<unsigned>("-1");
    EXPECT_FALSE(v);
}

TEST(SafeConversion, ParseDouble_Negative) {
    auto v = parse_to<double>("-0.001");
    ASSERT_TRUE(v);
    EXPECT_DOUBLE_EQ(*v, -0.001);
}

TEST(SafeConversion, ParseDouble_WhitespaceOnly) {
    auto v = parse_to<double>("   ");
    ASSERT_FALSE(v);
    EXPECT_EQ(v.error().errc, ConversionErrc::empty);
}

TEST(SafeConversion, ParseInt64_Ok) {
    auto v = parse_to<std::int64_t>("42");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 42);
}

TEST(SafeConversion, ParseUint64_Ok) {
    auto v = parse_to<std::uint64_t>("18446744073709551614");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 18446744073709551614ULL);
}

TEST(SafeConversion, ParseBool_CaseInsensitive) {
    EXPECT_TRUE(*parse_bool("TRUE"));
    EXPECT_TRUE(*parse_bool("Yes"));
    EXPECT_FALSE(*parse_bool("FALSE"));
    EXPECT_FALSE(*parse_bool("No"));
    EXPECT_FALSE(*parse_bool("N"));
}

TEST(SafeConversion, ParseBool_Empty) {
    auto v = parse_bool("");
    EXPECT_FALSE(v.has_value());
}

TEST(SafeConversion, ParseOrDefault_Empty) {
    EXPECT_DOUBLE_EQ(parse_or_default<double>("", "f", "@T", 99.9), 99.9);
}

TEST(SafeConversion, ParseOrDefault_Invalid) {
    EXPECT_EQ(parse_or_default<int>("abc", "f", "@T", -42), -42);
}

TEST(SafeConversion, ParseOrThrow_Double) {
    EXPECT_DOUBLE_EQ(parse_or_throw<double>("3.14", "f", "@T"), 3.14);
}

TEST(SafeConversion, ParseOrThrow_DoubleInvalid) {
    EXPECT_THROW(parse_or_throw<double>("abc", "f", "@T"), ate::ConversionError);
}

// ===================================================================
//  field_type: extended coverage
// ===================================================================

TEST(FieldValue, ParseFloat) {
    auto v = parse_field_value("3.14", FieldType::fp);
    ASSERT_TRUE(v);
    EXPECT_DOUBLE_EQ(std::get<double>(*v), 3.14);
}

TEST(FieldValue, ParseString) {
    auto v = parse_field_value("hello", FieldType::str);
    ASSERT_TRUE(v);
    EXPECT_EQ(std::get<std::string>(*v), "hello");
}

TEST(FieldValue, ParseUnknownType) {
    auto v = parse_field_value("data", FieldType::unknown);
    EXPECT_FALSE(v.has_value());
}

TEST(FieldValue, ParseListType) {
    auto v = parse_field_value("data", FieldType::list);
    EXPECT_FALSE(v.has_value());
}

TEST(FieldValue, ParseLiteralType) {
    auto v = parse_field_value("raw", FieldType::literal);
    EXPECT_FALSE(v.has_value());
}

TEST(FieldValue, TypeOfVariants) {
    EXPECT_EQ(type_of(FieldValue{true}),               FieldType::boolean);
    EXPECT_EQ(type_of(FieldValue{42}),                 FieldType::integer);
    EXPECT_EQ(type_of(FieldValue{3.14}),               FieldType::fp);
    EXPECT_EQ(type_of(FieldValue{std::string{"hi"}}),  FieldType::str);
}

TEST(FieldValue, ToFieldType_Str) {
    EXPECT_EQ(to_field_type("str"),     FieldType::str);
    EXPECT_EQ(to_field_type("STRING"),  FieldType::str);
}

TEST(FieldValue, ToFieldType_List) {
    EXPECT_EQ(to_field_type("list"),    FieldType::list);
    EXPECT_EQ(to_field_type("array"),   FieldType::list);
}

TEST(FieldValue, ToFieldType_Literal) {
    EXPECT_EQ(to_field_type("literal"), FieldType::literal);
    EXPECT_EQ(to_field_type("raw"),     FieldType::literal);
}

TEST(FieldValue, ToFieldType_Unknown) {
    EXPECT_EQ(to_field_type("xyz_gibberish"), FieldType::unknown);
}

TEST(FieldValue, FieldValueToString_Double) {
    auto s = field_value_to_string(FieldValue{2.5});
    EXPECT_FALSE(s.empty());
    // Should contain "2.5" or similar numeric representation
    EXPECT_NE(s.find("2.5"), std::string::npos);
}

TEST(FieldValue, FieldValueToString_Bool) {
    EXPECT_EQ(field_value_to_string(FieldValue{true}), "1");
    EXPECT_EQ(field_value_to_string(FieldValue{false}), "0");
}

TEST(FieldValue, ParseBoolField_False) {
    auto v = parse_field_value("no", FieldType::boolean);
    ASSERT_TRUE(v);
    EXPECT_FALSE(std::get<bool>(*v));
}

TEST(FieldValue, ParseBoolField_Invalid) {
    auto v = parse_field_value("maybe", FieldType::boolean);
    EXPECT_FALSE(v);
}

TEST(FieldValue, ParseFloat_Invalid) {
    auto v = parse_field_value("not_a_number", FieldType::fp);
    EXPECT_FALSE(v);
}

TEST(FieldValue, ParseFloat_Scientific) {
    auto v = parse_field_value("1.5E+02", FieldType::fp);
    ASSERT_TRUE(v);
    EXPECT_DOUBLE_EQ(std::get<double>(*v), 150.0);
}

TEST(FieldType, ToStringAll) {
    EXPECT_EQ(to_string(FieldType::boolean), "bool");
    EXPECT_EQ(to_string(FieldType::integer), "int");
    EXPECT_EQ(to_string(FieldType::fp),      "fp");
    EXPECT_EQ(to_string(FieldType::str),     "str");
    EXPECT_EQ(to_string(FieldType::list),    "list");
    EXPECT_EQ(to_string(FieldType::literal), "literal");
    EXPECT_EQ(to_string(FieldType::unknown), "unknown");
}

// ===================================================================
//  Error types: extended coverage
// ===================================================================

TEST(Errors, MalformedRecordError) {
    try {
        throw ate::MalformedRecordError("unbalanced braces at pos 42");
    } catch (const ate::ParseError& e) {
        EXPECT_NE(std::string(e.message()).find("unbalanced"), std::string::npos);
    }
}

TEST(Errors, IoError) {
    try {
        throw ate::IoError("file not found: test.log");
    } catch (const ate::ParseError& e) {
        EXPECT_NE(std::string(e.message()).find("file not found"), std::string::npos);
    }
}

TEST(Errors, IntegrityError) {
    try {
        throw ate::IntegrityError("no @BATCH record");
    } catch (const ate::ParseError& e) {
        auto& trace = e.trace();
        EXPECT_GT(trace.frames.size(), 0u);
    }
}

TEST(Errors, ConversionErrorFields) {
    ate::ConversionError err("raw_val", "double", "measured", "@A-RES");
    EXPECT_EQ(err.raw(), "raw_val");
    EXPECT_EQ(err.target_type(), "double");
    EXPECT_EQ(err.field_name(), "measured");
    EXPECT_EQ(err.record_type(), "@A-RES");
    std::string msg(err.what());
    EXPECT_NE(msg.find("raw_val"), std::string::npos);
    EXPECT_NE(msg.find("double"), std::string::npos);
}
