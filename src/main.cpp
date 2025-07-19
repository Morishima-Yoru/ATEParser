#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "keysight_log/core/I3070LogParser.hpp"
#include "keysight_log/core/LogRecord.hpp"
#include "keysight_log/utils/ConfigReader.hpp"

int main(int argc, char* argv[]) {
    keysight_log::core::ConfigReader config("config.ini");
    keysight_log::core::LogRecord::show_raw_field = config.getBool("DEBUG", "SHOW_RAW", false);
    
    // Read JSON indent size from config
    int indent_size = config.getInt("EXPORT", "INDENT_SIZE", 2);
    
    // Read log file
    std::string input_file = argv[1];
    std::ifstream ifs(input_file);
    if (!ifs) {
        std::cerr << "Cannot open log file" << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string logText = buffer.str();

    // Create parser and parse log
    keysight_log::core::I3070LogParser parser;
    auto tree = parser.parse(logText);
    // Output JSON with configurable indent
    auto json = keysight_log::core::I3070LogParser::containerToJson(*tree);
    
    std::string output_file = input_file.substr(0, input_file.find_last_of('.')) + ".json";
    std::ofstream ofs(output_file);
    ofs << json.dump(indent_size);
    ofs.close();
    return 0;
}
