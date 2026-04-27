/**
 * @file test_field_value.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/enums/field_type.hpp"

using namespace ate::enums;

TEST(FieldValue, RoundTrip) {
    FieldValue b = true;
    FieldValue i = 42;
    FieldValue d = 3.14;
    FieldValue s = std::string{"hi"};
    EXPECT_EQ(type_tag(b), 'b');
    EXPECT_EQ(type_tag(i), 'i');
    EXPECT_EQ(type_tag(d), 'f');
    EXPECT_EQ(type_tag(s), 's');
    EXPECT_EQ(field_value_to_string(b), "1");
    EXPECT_EQ(field_value_to_string(i), "42");
    EXPECT_EQ(field_value_to_string(s), "hi");
}

TEST(FieldValue, ParseStringToValue) {
    auto v = parse_field_value("123", FieldType::integer);
    ASSERT_TRUE(v);
    EXPECT_EQ(std::get<int>(*v), 123);

    auto v2 = parse_field_value("yes", FieldType::boolean);
    ASSERT_TRUE(v2);
    EXPECT_TRUE(std::get<bool>(*v2));

    auto v3 = parse_field_value("not-a-number", FieldType::integer);
    EXPECT_FALSE(v3);
}

TEST(FieldValue, FieldTypeStrings) {
    EXPECT_EQ(to_field_type("int"),    FieldType::integer);
    EXPECT_EQ(to_field_type("FLOAT"),  FieldType::fp);
    EXPECT_EQ(to_field_type("Bool"),   FieldType::boolean);
    EXPECT_EQ(to_string(FieldType::str), "str");
}
