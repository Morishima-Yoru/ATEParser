/**
 * @file test_errors.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/utils/errors.hpp"

TEST(Errors, ConversionErrorCarriesContext) {
    try {
        throw ate::ConversionError("abc", "int", "field_x", "@T");
    } catch (const ate::ConversionError& e) {
        EXPECT_EQ(e.raw(), "abc");
        EXPECT_EQ(e.target_type(), "int");
        EXPECT_EQ(e.field_name(), "field_x");
        EXPECT_EQ(e.record_type(), "@T");
        EXPECT_NE(std::string(e.what()).find("abc"), std::string::npos);
    }
}

TEST(Errors, HierarchyAndTrace) {
    try {
        throw ate::IntegrityError("missing batch");
    } catch (const ate::ParseError& e) {
        EXPECT_NE(std::string(e.message()).find("missing batch"), std::string::npos);
        const auto& trace = e.trace();
        EXPECT_GT(trace.frames.size(), 0u);
    }
}
