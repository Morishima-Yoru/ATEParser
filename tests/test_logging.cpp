/**
 * @file test_logging.cpp
 */
#include <gtest/gtest.h>
#include "ate_parser/utils/logging.hpp"

TEST(Logging, LoggerSingleton) {
    auto& a = ate::log::logger();
    auto& b = ate::log::logger();
    EXPECT_EQ(a.get(), b.get());
    EXPECT_EQ(a->name(), "ate");
}

TEST(Logging, MacrosCompileAndEmit) {
    ATE_LOG_INFO("hello {}", 42);
    ATE_LOG_WARN("warning {}", "test");
    ATE_LOG_ERROR("error {}", 3.14);
    SUCCEED();
}
