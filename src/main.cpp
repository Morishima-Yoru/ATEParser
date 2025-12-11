#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include "i3070/core/I3070LogParser.hpp"
#include "i3070/core/LogRecord.hpp"
#include "i3070/utils/SafeConversion.hpp"

namespace fs = std::filesystem;

class ArgParser {
public:
    struct Config {
        bool show_raw = false;
        bool show_parser = false;
        bool show_unimplemented = false;
        bool show_conversion = false;
        int indent_size = 2;
        std::string input_file;
        std::string output_file;
    };

    ArgParser() {
        registerFlag("--raw", [](Config& c) { c.show_raw = true; });
        registerFlag("--show-parser", [](Config& c) { c.show_parser = true; });
        registerFlag("--show-unimplemented", [](Config& c) { c.show_unimplemented = true; });
        registerFlag("--show-conversion", [](Config& c) { c.show_conversion = true; });
        registerFlag("--help", [](Config& c) { printUsage("ATEParserApp"); exit(0); });
        
        registerOption("--indent", [](Config& c, const std::string& val) {
            try {
                c.indent_size = std::stoi(val);
            } catch (...) {
                std::cerr << "Invalid indent size provided.\n";
                exit(1);
            }
        });
        registerOption("--output", [](Config& c, const std::string& val) {
            c.output_file = val;
        });
    }

    Config parse(int argc, char* argv[]) {
        Config config;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // 1. Handle Flags
            if (flags_.count(arg)) {
                flags_[arg](config);
                continue;
            } 
            
            // 2. Handle Options
            if (options_.count(arg)) {
                if (i + 1 >= argc) {
                    std::cerr << "Missing value for " << arg << ".\n";
                    exit(1);
                }
                options_[arg](config, argv[++i]);
                continue;
            } 
            
            // 3. Handle Input File (or unknown arg)
            if (config.input_file.empty()) {
                config.input_file = arg;
                continue;
            }
            
            // 4. Unknown Argument
            std::cerr << "Unknown argument: " << arg << "\n";
            printUsage(argv[0]);
            exit(1);
        }
        return config;
    }

    static void printUsage(const char* programName) {
        std::cerr << "Usage: " << programName << " [options] <logfile>\n"
                  << "Options:\n"
                  << "  --raw                 Keep raw log fields in JSON output (default: false)\n"
                  << "  --indent <N>          Set JSON indentation size (default: 2)\n"
                  << "  --output <file>       Specify output JSON file path\n"
                  << "  --show-parser         Enable parser trace output\n"
                  << "  --show-unimplemented  Enable unimplemented prefix warnings\n"
                  << "  --show-conversion     Enable type conversion debug logs\n"
                  << "  --help                Show this help message\n";
    }

private:
    using FlagHandler = std::function<void(Config&)>;
    using OptionHandler = std::function<void(Config&, const std::string&)>;

    std::unordered_map<std::string, FlagHandler> flags_;
    std::unordered_map<std::string, OptionHandler> options_;

    void registerFlag(const std::string& name, FlagHandler handler) {
        flags_[name] = handler;
    }

    void registerOption(const std::string& name, OptionHandler handler) {
        options_[name] = handler;
    }
};

int main(int argc, char* argv[]) {
    try {
        if (argc < 2) {
            ArgParser::printUsage(argv[0]);
            return 1;
        }

        ArgParser parser;
        auto config = parser.parse(argc, argv);

        if (config.input_file.empty()) {
            std::cerr << "No input file specified.\n";
            ArgParser::printUsage(argv[0]);
            return 1;
        }

        // Apply configuration
        i3070::core::LogRecord::show_raw_field = config.show_raw;
        i3070::core::I3070LogParser::show_parser_debug = config.show_parser;
        i3070::core::LogRecord::show_unimplemented_prefix = config.show_unimplemented;
        i3070::core::show_conversion_debug = config.show_conversion;

        // Read log file
        std::ifstream ifs(config.input_file);
        if (!ifs) {
            std::cerr << "Cannot open log file: " << config.input_file << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        std::string logText = buffer.str();

        // Create parser and parse log
        i3070::core::I3070LogParser logParser;
        auto tree = logParser.parse(logText);
        
        // Output JSON with configurable indent
        auto json = i3070::core::I3070LogParser::containerToJson(*tree);
        
        std::string final_output = config.output_file;
        if (final_output.empty()) {
            // Drag-and-drop or default behavior: output in same directory as input
            fs::path inputPath(config.input_file);
            final_output = (inputPath.parent_path() / inputPath.stem()).string() + ".json";
        }

        std::ofstream ofs(final_output);
        if (!ofs) {
             std::cerr << "Cannot open output file: " << final_output << std::endl;
             return 1;
        }
        ofs << json.dump(config.indent_size);
        ofs.close();

        std::cout << "Successfully converted " << config.input_file << " to " << final_output << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}
