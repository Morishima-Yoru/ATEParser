#include <gtest/gtest.h>
#include "i3070/core/I3070LogParser.hpp"

using namespace i3070::core;

TEST(ParserTest, BasicParsing) {
    // Placeholder test to verify GTest setup
    EXPECT_TRUE(true);
}

TEST(ParserTest, ParseFields) {
    std::string record = "@A-CAP|1|1.246700E+01|C1";
    auto result = I3070LogParser::parseFields(record);
    
    EXPECT_EQ(result.first, "@A-CAP");
    ASSERT_EQ(result.second.size(), 3);
    EXPECT_EQ(result.second[0], "1");
    EXPECT_EQ(result.second[1], "1.246700E+01");
    EXPECT_EQ(result.second[2], "C1");
}

TEST(ParserTest, ParseFieldsWithEmpty) {
    std::string record = "@A-CAP|1||C1";
    auto result = I3070LogParser::parseFields(record);
    
    EXPECT_EQ(result.first, "@A-CAP");
    ASSERT_EQ(result.second.size(), 3);
    EXPECT_EQ(result.second[0], "1");
    EXPECT_EQ(result.second[1], "");
    EXPECT_EQ(result.second[2], "C1");
}
