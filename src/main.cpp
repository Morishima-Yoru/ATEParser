/**
 * @file main.cpp
 * @brief Command-line entry point: ATEParserApp.
 */
#include "ate_parser/ate_parser.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

namespace {

struct CliOptions {
    bool        keep_raw    = false;
    int         indent_size = 2;
    bool        debug       = false;
    std::string input_file;
    std::string output_file;
};

void print_usage(std::string_view program) {
    std::cerr << "Usage: " << program << " [options] <logfile>\n"
              << "Options:\n"
              << "  --raw            Keep raw flat-field text in JSON output\n"
              << "  --indent <N>     JSON indentation size (default: 2)\n"
              << "  --output <path>  Output JSON file path\n"
              << "  --debug          Verbose logging\n"
              << "  --help           Show this help message\n";
}

CliOptions parse_args(int argc, char* argv[]) {
    CliOptions opts;
    for (int i = 1; i < argc; ++i) {
        std::string_view a = argv[i];
        if      (a == "--raw")    opts.keep_raw = true;
        else if (a == "--debug")  opts.debug    = true;
        else if (a == "--help")   { print_usage(argv[0]); std::exit(0); }
        else if (a == "--indent") {
            if (i + 1 >= argc) { std::cerr << "Missing value for --indent\n"; std::exit(1); }
            try { opts.indent_size = std::stoi(argv[++i]); }
            catch (...) { std::cerr << "Invalid --indent value\n"; std::exit(1); }
        } else if (a == "--output") {
            if (i + 1 >= argc) { std::cerr << "Missing value for --output\n"; std::exit(1); }
            opts.output_file = argv[++i];
        } else if (!a.empty() && a.front() == '-') {
            std::cerr << "Unknown argument: " << a << "\n";
            print_usage(argv[0]);
            std::exit(1);
        } else if (opts.input_file.empty()) {
            opts.input_file = a;
        } else {
            std::cerr << "Unexpected positional argument: " << a << "\n";
            std::exit(1);
        }
    }
    return opts;
}

} // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) { print_usage(argv[0]); return 1; }
        auto opts = parse_args(argc, argv);
        if (opts.input_file.empty()) {
            std::cerr << "No input file specified.\n";
            print_usage(argv[0]);
            return 1;
        }
        if (opts.debug) ate::log::set_level(spdlog::level::debug);

        std::ifstream ifs(opts.input_file);
        if (!ifs) {
            std::cerr << "Cannot open log file: " << opts.input_file << '\n';
            return 1;
        }
        std::stringstream buf;
        buf << ifs.rdbuf();

        ate::core::Parser parser;
        auto tree = parser.parse(buf.str());

        nlohmann::json j;
        if (!tree.children.empty()) j = ate::core::to_json(tree.children.front(), opts.keep_raw);
        else                        j = nlohmann::json::object();

        std::string out_path = opts.output_file;
        if (out_path.empty()) {
            fs::path p(opts.input_file);
            out_path = (p.parent_path() / p.stem()).string() + ".json";
        }
        std::ofstream ofs(out_path);
        if (!ofs) {
            std::cerr << "Cannot open output file: " << out_path << '\n';
            return 1;
        }
        ofs << j.dump(opts.indent_size);
        std::cout << "Successfully converted " << opts.input_file << " to " << out_path << '\n';
        return 0;
    } catch (const ate::ParseError& e) {
        std::cerr << "ParseError: " << e.message() << '\n';
        std::cerr << e.trace().to_string() << '\n';
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown error.\n";
        return 1;
    }
}