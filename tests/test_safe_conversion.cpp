/**
 * @file test_safe_conversion.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/utils/safe_conversion.hpp"
#include "ate_parser/utils/errors.hpp"

using namespace ate::utils;

TEST(SafeConversion, ParseInt_Ok) {
    auto v = parse_to<int>("42");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 42);
}

TEST(SafeConversion, ParseInt_Trim) {
    auto v = parse_to<int>("  -7 ");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, -7);
}

TEST(SafeConversion, ParseInt_Invalid) {
    auto v = parse_to<int>("abc");
    ASSERT_FALSE(v);
    EXPECT_EQ(v.error().errc, ConversionErrc::invalid_syntax);
}

TEST(SafeConversion, ParseInt_Empty) {
    auto v = parse_to<int>("");
    ASSERT_FALSE(v);
    EXPECT_EQ(v.error().errc, ConversionErrc::empty);
}

TEST(SafeConversion, ParseDouble_Ok) {
    auto v = parse_to<double>("3.14");
    ASSERT_TRUE(v);
    EXPECT_DOUBLE_EQ(*v, 3.14);
}

TEST(SafeConversion, ParseUll_Ok) {
    auto v = parse_to<unsigned long long>("18446744073709551615");
    ASSERT_TRUE(v);
    EXPECT_EQ(*v, 18446744073709551615ULL);
}

TEST(SafeConversion, ParseInt_OutOfRange) {
    auto v = parse_to<int>("99999999999999");
    ASSERT_FALSE(v);
    EXPECT_EQ(v.error().errc, ConversionErrc::out_of_range);
}

TEST(SafeConversion, ParseBool_Variants) {
    EXPECT_TRUE(*parse_bool("1"));
    EXPECT_TRUE(*parse_bool("Y"));
    EXPECT_TRUE(*parse_bool("yes"));
    EXPECT_TRUE(*parse_bool("True"));
    EXPECT_FALSE(*parse_bool("0"));
    EXPECT_FALSE(*parse_bool("no"));
    EXPECT_FALSE(parse_bool("maybe").has_value());
}

TEST(SafeConversion, ParseOrThrow_Throws) {
    EXPECT_THROW(parse_or_throw<int>("xyz", "f", "@T"), ate::ConversionError);
}

TEST(SafeConversion, ParseOrThrow_Ok) {
    EXPECT_EQ(parse_or_throw<int>("123", "f", "@T"), 123);
}

TEST(SafeConversion, ParseOrDefault) {
    EXPECT_EQ(parse_or_default<int>("",   "f", "@T", -1), -1);
    EXPECT_EQ(parse_or_default<int>("xx", "f", "@T", -1), -1);
    EXPECT_EQ(parse_or_default<int>("99", "f", "@T", -1), 99);
}

TEST(SafeConversion, ParseBoolOrDefault) {
    EXPECT_TRUE(parse_bool_or_default("",    "f", "@T", true));
    EXPECT_FALSE(parse_bool_or_default("",   "f", "@T", false));
    EXPECT_TRUE(parse_bool_or_default("yes", "f", "@T", false));
}
