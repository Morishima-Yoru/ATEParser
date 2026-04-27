/**
 * @file test_benchmark.cpp
 * @brief Simple parse-throughput benchmark (not a correctness test).
 *
 * Reads one of the example log files from .example_logs/_processed/ and
 * runs parse() + to_json() + dump() in a tight loop, printing elapsed
 * time per iteration.  The test is DISABLED by default (prefixed with
 * DISABLED_) so it does not slow CI; run it explicitly with:
 *
 *     ./unit_tests --gtest_also_run_disabled_tests --gtest_filter=Benchmark.*
 */
#include <gtest/gtest.h>
#include "ate_parser/ate_parser.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace ate::core;
namespace fs = std::filesystem;

namespace {

/// Try a few well-known relative paths to locate a sample log.
std::string load_sample_log() {
    const char* env = std::getenv("ATE_BENCHMARK_LOG");
    std::vector<fs::path> candidates;
    if (env) candidates.emplace_back(env);
    candidates.emplace_back("../../.example_logs/_processed/P1086592-22-L_AAAA00000000001_260323035023_MY59250383_Pass.txt");
    candidates.emplace_back("../.example_logs/_processed/P1086592-22-L_AAAA00000000001_260323035023_MY59250383_Pass.txt");
    candidates.emplace_back("../../../.example_logs/_processed/P1086592-22-L_AAAA00000000001_260323035023_MY59250383_Pass.txt");

    for (auto& p : candidates) {
        if (fs::exists(p)) {
            std::ifstream ifs(p);
            std::stringstream buf;
            buf << ifs.rdbuf();
            return buf.str();
        }
    }
    return {};
}

} // namespace

TEST(Benchmark, DISABLED_ParseAndDump) {
    auto log = load_sample_log();
    if (log.empty()) {
        GTEST_SKIP() << "No sample log found; set ATE_BENCHMARK_LOG env var";
    }

    constexpr int kIterations = 100;
    Parser parser;

    // Warm-up
    {
        auto tree = parser.parse(log);
        auto j = to_json(tree.children.front());
        (void)j.dump(-1);
    }

    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();

    for (int i = 0; i < kIterations; ++i) {
        auto tree = parser.parse(log);
        auto j = to_json(tree.children.front());
        auto s = j.dump(-1);
        // Prevent optimising away the result.
        if (s.empty()) std::abort();
    }

    auto t1 = clock::now();
    double total_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double per_iter = total_ms / kIterations;

    std::cout << "\n=== Benchmark: parse + to_json + dump ===\n"
              << "  Iterations : " << kIterations << "\n"
              << "  Total      : " << total_ms << " ms\n"
              << "  Per iter   : " << per_iter << " ms\n"
              << "  Log size   : " << log.size() << " bytes\n";
}

TEST(Benchmark, DISABLED_ParseOnly) {
    auto log = load_sample_log();
    if (log.empty()) {
        GTEST_SKIP() << "No sample log found; set ATE_BENCHMARK_LOG env var";
    }

    constexpr int kIterations = 100;
    Parser parser;

    // Warm-up
    { auto tree = parser.parse(log); (void)tree; }

    using clock = std::chrono::high_resolution_clock;
    auto t0 = clock::now();

    for (int i = 0; i < kIterations; ++i) {
        auto tree = parser.parse(log);
        if (tree.children.empty()) std::abort();
    }

    auto t1 = clock::now();
    double total_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double per_iter = total_ms / kIterations;

    std::cout << "\n=== Benchmark: parse only ===\n"
              << "  Iterations : " << kIterations << "\n"
              << "  Total      : " << total_ms << " ms\n"
              << "  Per iter   : " << per_iter << " ms\n"
              << "  Log size   : " << log.size() << " bytes\n";
}
