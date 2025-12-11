#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "i3070/core/I3070LogParser.hpp"
#include "i3070/core/LogRecord.hpp"
#include "i3070/utils/ConfigReader.hpp"

int main(int argc, char* argv[]) {
    i3070::core::ConfigReader config("config.ini");
    i3070::core::LogRecord::show_raw_field = config.getBool("DEBUG", "SHOW_RAW", false);
    
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
    i3070::core::I3070LogParser parser;
    auto tree = parser.parse(logText);
    // Output JSON with configurable indent
    auto json = i3070::core::I3070LogParser::containerToJson(*tree);
    
    std::string output_file = input_file.substr(0, input_file.find_last_of('.')) + ".json";
    std::ofstream ofs(output_file);
    ofs << json.dump(indent_size);
    ofs.close();
    return 0;
}
