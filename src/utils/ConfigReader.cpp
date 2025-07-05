#include "keysight_log/utils/ConfigReader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using namespace keysight_log::core;
using namespace std;
using json = nlohmann::json;

ConfigReader::ConfigReader(const std::string& filename) {
    parse(filename);
}

void ConfigReader::parse(const string& filename) {
    ifstream ifs(filename);
    string line, section;
    while (getline(ifs, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
        if (line.front() == '[' && line.back() == ']') {
            section = line.substr(1, line.size() - 2);
        } else {
            auto eq = line.find('=');
            if (eq != string::npos) {
                string key = line.substr(0, eq);
                string value = line.substr(eq + 1);
                data_[section][key] = value;
            }
        }
    }
}

bool ConfigReader::getBool(const string& section, const string& key, bool defaultValue) const {
    auto secIt = data_.find(section);
    if (secIt != data_.end()) {
        auto keyIt = secIt->second.find(key);
        if (keyIt != secIt->second.end()) {
            string val = keyIt->second;
            transform(val.begin(), val.end(), val.begin(), ::tolower);
            return val == "true" || val == "1" || val == "yes";
        }
    }
    return defaultValue;
}

int ConfigReader::getInt(const string& section, const string& key, int defaultValue) const {
    auto secIt = data_.find(section);
    if (secIt != data_.end()) {
        auto keyIt = secIt->second.find(key);
        if (keyIt != secIt->second.end()) {
            try {
                return stoi(keyIt->second);
            } catch (const exception& e) {
                // Return default value if conversion fails
                return defaultValue;
            }
        }
    }
    return defaultValue;
} 