/**
 * @file main.cpp
 * @brief Command-line entry point: ATEParserApp.
 */
#include "ate_parser/ate_parser.hpp"
#include "ate_parser/utils/cli_messages.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace fs  = std::filesystem;
namespace cli_msg = ate::cli_msg;

namespace {

struct CliOptions {
    bool        keep_raw    = false;
    int         indent_size = 2;
    bool        debug       = false;
    std::string input_file;
    std::string output_file;
};

void print_usage(std::string_view program) {
    std::cerr << cli_msg::k_usage_invocation_prefix << program << cli_msg::k_usage_invocation_args
              << cli_msg::k_usage_header
              << cli_msg::k_usage_raw
              << cli_msg::k_usage_indent
              << cli_msg::k_usage_output
              << cli_msg::k_usage_debug
              << cli_msg::k_usage_help;
}

CliOptions parse_args(int argc, char* argv[]) {
    CliOptions opts;
    for (int i = 1; i < argc; ++i) {
        std::string_view a = argv[i];
        if      (a == cli_msg::k_arg_raw)    opts.keep_raw = true;
        else if (a == cli_msg::k_arg_debug)  opts.debug    = true;
        else if (a == cli_msg::k_arg_help)   { print_usage(argv[0]); std::exit(0); }
        else if (a == cli_msg::k_arg_indent) {
            if (i + 1 >= argc) { std::cerr << cli_msg::k_err_missing_indent; std::exit(1); }
            try { opts.indent_size = std::stoi(argv[++i]); }
            catch (...) { std::cerr << cli_msg::k_err_invalid_indent; std::exit(1); }
        } else if (a == cli_msg::k_arg_output) {
            if (i + 1 >= argc) { std::cerr << cli_msg::k_err_missing_output; std::exit(1); }
            opts.output_file = argv[++i];
        } else if (!a.empty() && a.front() == '-') {
            std::cerr << cli_msg::k_err_unknown_arg << a << "\n";
            print_usage(argv[0]);
            std::exit(1);
        } else if (opts.input_file.empty()) {
            opts.input_file = a;
        } else {
            std::cerr << cli_msg::k_err_unexpected_positional << a << "\n";
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
            std::cerr << cli_msg::k_err_no_input;
            print_usage(argv[0]);
            return 1;
        }
        if (opts.debug) ate::log::set_level(spdlog::level::debug);

        std::ifstream ifs(opts.input_file);
        if (!ifs) {
            std::cerr << cli_msg::k_err_open_log << opts.input_file << '\n';
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
            std::cerr << cli_msg::k_err_open_output << out_path << '\n';
            return 1;
        }
        ofs << j.dump(opts.indent_size);
        std::cout << cli_msg::k_msg_converted_prefix << opts.input_file << cli_msg::k_msg_converted_to << out_path << '\n';
        return 0;
    } catch (const ate::ParseError& e) {
        std::cerr << cli_msg::k_err_parse_error << e.message() << '\n';
        std::cerr << e.trace().to_string() << '\n';
        return 2;
    } catch (const std::exception& e) {
        std::cerr << cli_msg::k_err_generic << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << cli_msg::k_err_unknown;
        return 1;
    }
}
