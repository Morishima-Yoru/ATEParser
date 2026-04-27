/**
 * @file bench_parse.cpp
 * @brief Micro-benchmark for ATEParser parse path and targeted optimizations.
 *
 * Usage: bench_parse <logfile> [iterations]
 */
#include "ate_parser/core/parser.hpp"
#include "ate_parser/core/record_tree.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

using Clock = std::chrono::high_resolution_clock;

struct PipelineBenchResult {
    double parse_ms;
    double total_ms;
    double json_ms;
    double parse_ms_per_iter;
    double total_ms_per_iter;
    double json_ms_per_iter;
};

struct AtoBBenchResult {
    double before_ms;
    double after_ms;
    double improvement_rate_pct;
};

[[nodiscard]] double calc_improvement_rate(double before_ms, double after_ms) {
    if (before_ms <= 0.0) return 0.0;
    return (before_ms - after_ms) / before_ms * 100.0;
}

[[nodiscard]] std::string_view trim_view(std::string_view sv) {
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.front()))) {
        sv.remove_prefix(1);
    }
    while (!sv.empty() && std::isspace(static_cast<unsigned char>(sv.back()))) {
        sv.remove_suffix(1);
    }
    return sv;
}

[[nodiscard]] bool parse_bool_old(std::string_view raw) {
    auto sv = trim_view(raw);
    if (sv.empty()) return false;

    std::string lower(sv);
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (lower == "1" || lower == "y" || lower == "yes" || lower == "true") return true;
    if (lower == "0" || lower == "n" || lower == "no" || lower == "false") return false;
    return false;
}

[[nodiscard]] bool parse_bool_new(std::string_view raw) {
    auto sv = trim_view(raw);
    if (sv.empty()) return false;

    auto iequals = [](std::string_view a, std::string_view b) {
        if (a.size() != b.size()) return false;
        for (std::size_t i = 0; i < a.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(a[i])) !=
                std::tolower(static_cast<unsigned char>(b[i]))) {
                return false;
            }
        }
        return true;
    };

    if (sv == "1" || iequals(sv, "y") || iequals(sv, "yes") || iequals(sv, "true")) return true;
    if (sv == "0" || iequals(sv, "n") || iequals(sv, "no") || iequals(sv, "false")) return false;
    return false;
}

[[nodiscard]] int parse_rpt_literal_len_old(std::string_view rpt_text) {
    auto tilde = rpt_text.find('~');
    auto pipe = rpt_text.find('|', tilde);
    std::string len_str{rpt_text.substr(tilde + 1, pipe - tilde - 1)};
    return std::stoi(len_str);
}

[[nodiscard]] int parse_rpt_literal_len_new(std::string_view rpt_text) {
    auto tilde = rpt_text.find('~');
    auto pipe = rpt_text.find('|', tilde);
    auto len_sv = rpt_text.substr(tilde + 1, pipe - tilde - 1);
    int literal_length = 0;
    auto [ptr, ec] = std::from_chars(len_sv.data(), len_sv.data() + len_sv.size(), literal_length);
    if (ec != std::errc{} || ptr != len_sv.data() + len_sv.size()) return 0;
    return literal_length;
}

[[nodiscard]] PipelineBenchResult benchmark_pipeline(const std::string& text, int iterations) {
    // Warm-up
    for (int i = 0; i < 3; ++i) {
        ate::core::Parser parser;
        auto tree = parser.parse(text, false);
        auto j = ate::core::to_json(tree.children.front(), false);
        auto s = j.dump(-1);
        (void)s;
    }

    auto t0 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        ate::core::Parser parser;
        auto tree = parser.parse(text, false);
        (void)tree;
    }
    auto t1 = Clock::now();

    auto t2 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        ate::core::Parser parser;
        auto tree = parser.parse(text, false);
        auto j = ate::core::to_json(tree.children.front(), false);
        auto s = j.dump(-1);
        (void)s;
    }
    auto t3 = Clock::now();

    const double parse_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double total_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();
    const double json_ms = total_ms - parse_ms;

    return {
        parse_ms,
        total_ms,
        json_ms,
        parse_ms / iterations,
        total_ms / iterations,
        json_ms / iterations,
    };
}

[[nodiscard]] AtoBBenchResult benchmark_keep_raw_effect(const std::string& text, int iterations) {
    volatile std::size_t sink = 0;

    auto t0 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        ate::core::Parser parser;
        auto tree = parser.parse(text, true);
        sink += tree.children.size();
    }
    auto t1 = Clock::now();

    auto t2 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        ate::core::Parser parser;
        auto tree = parser.parse(text, false);
        sink += tree.children.size();
    }
    auto t3 = Clock::now();

    (void)sink;
    const double before_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double after_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    return {before_ms, after_ms, calc_improvement_rate(before_ms, after_ms)};
}

[[nodiscard]] AtoBBenchResult benchmark_rpt_length_parse(int iterations) {
    constexpr std::array<std::string_view, 4> kRptSamples = {
        "@RPT~5|HELLO|EXTRA|FIELDS",
        "@RPT~12|HELLO_WORLD!|TAIL",
        "@RPT~3|ABC|REST",
        "@RPT~9|123456789|NEXT",
    };

    volatile int sink = 0;

    auto t0 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        for (auto sample : kRptSamples) sink += parse_rpt_literal_len_old(sample);
    }
    auto t1 = Clock::now();

    auto t2 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        for (auto sample : kRptSamples) sink += parse_rpt_literal_len_new(sample);
    }
    auto t3 = Clock::now();

    (void)sink;
    const double before_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double after_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    return {before_ms, after_ms, calc_improvement_rate(before_ms, after_ms)};
}

[[nodiscard]] AtoBBenchResult benchmark_parse_bool(int iterations) {
    constexpr std::array<std::string_view, 8> kBoolSamples = {
        "true", "FALSE", " Yes ", "no", "1", "0", "y", "N"
    };

    volatile int sink = 0;

    auto t0 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        for (auto sample : kBoolSamples) sink += static_cast<int>(parse_bool_old(sample));
    }
    auto t1 = Clock::now();

    auto t2 = Clock::now();
    for (int i = 0; i < iterations; ++i) {
        for (auto sample : kBoolSamples) sink += static_cast<int>(parse_bool_new(sample));
    }
    auto t3 = Clock::now();

    (void)sink;
    const double before_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
    const double after_ms = std::chrono::duration<double, std::milli>(t3 - t2).count();

    return {before_ms, after_ms, calc_improvement_rate(before_ms, after_ms)};
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <logfile> [iterations]\n";
        return 1;
    }

    const int iterations = (argc >= 3) ? std::atoi(argv[2]) : 200;
    const int micro_iterations = iterations * 3000;

    std::ifstream ifs(argv[1]);
    if (!ifs) {
        std::cerr << "Cannot open: " << argv[1] << '\n';
        return 1;
    }
    std::stringstream buf;
    buf << ifs.rdbuf();
    const std::string text = buf.str();
    if (text.empty()) {
        std::cerr << "Empty file\n";
        return 1;
    }

    const auto pipeline = benchmark_pipeline(text, iterations);
    const auto keep_raw = benchmark_keep_raw_effect(text, iterations);
    const auto rpt_parse = benchmark_rpt_length_parse(micro_iterations);
    const auto bool_parse = benchmark_parse_bool(micro_iterations);
    const double file_kb  = static_cast<double>(text.size()) / 1024.0;

    std::cout << "file_bytes=" << text.size()
              << "  iterations=" << iterations << '\n';
    std::cout << "parse_only: "
              << pipeline.parse_ms << " ms total, "
              << pipeline.parse_ms_per_iter << " ms/iter\n";
    std::cout << "parse+json: "
              << pipeline.total_ms << " ms total, "
              << pipeline.total_ms_per_iter << " ms/iter\n";
    std::cout << "json_delta: "
              << pipeline.json_ms  << " ms total, "
              << pipeline.json_ms_per_iter << " ms/iter\n";
    std::cout << "throughput_parse: "
              << (file_kb * iterations) / (pipeline.parse_ms / 1000.0) << " KB/s\n";
    std::cout << "throughput_total: "
              << (file_kb * iterations) / (pipeline.total_ms / 1000.0) << " KB/s\n";

    std::cout << "\n# Improvement rates (before -> after)\n";
    std::cout << "keep_raw_copy: "
              << keep_raw.before_ms << " ms -> "
              << keep_raw.after_ms << " ms, "
              << keep_raw.improvement_rate_pct << "%\n";
    std::cout << "rpt_len_parse: "
              << rpt_parse.before_ms << " ms -> "
              << rpt_parse.after_ms << " ms, "
              << rpt_parse.improvement_rate_pct << "%\n";
    std::cout << "parse_bool: "
              << bool_parse.before_ms << " ms -> "
              << bool_parse.after_ms << " ms, "
              << bool_parse.improvement_rate_pct << "%\n";

    std::cout << "\n# Markdown table\n";
    std::cout << "| Improvement | Before (ms) | After (ms) | Improvement Rate |\n";
    std::cout << "|---|---:|---:|---:|\n";
    std::cout << "| keep_raw conditional copy | "
              << keep_raw.before_ms << " | "
              << keep_raw.after_ms << " | "
              << keep_raw.improvement_rate_pct << "% |\n";
    std::cout << "| @RPT literal length parse (stoi -> from_chars) | "
              << rpt_parse.before_ms << " | "
              << rpt_parse.after_ms << " | "
              << rpt_parse.improvement_rate_pct << "% |\n";
    std::cout << "| parse_bool lower-copy removal | "
              << bool_parse.before_ms << " | "
              << bool_parse.after_ms << " | "
              << bool_parse.improvement_rate_pct << "% |\n";

    return 0;
}
