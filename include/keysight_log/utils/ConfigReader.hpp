#ifndef KEYSIGHT_LOG_CORE_CONFIGREADER_HPP
#define KEYSIGHT_LOG_CORE_CONFIGREADER_HPP

#include <string>
#include <unordered_map>

using namespace std;

namespace keysight_log {
namespace core {

class ConfigReader {
public:
    ConfigReader(const string& filename);
    bool getBool(const string& section, const string& key, bool defaultValue = false) const;
    int getInt(const string& section, const string& key, int defaultValue = 0) const;
private:
    unordered_map<string, unordered_map<string, string>> data_;
    void parse(const string& filename);
};

} // namespace core
} // namespace keysight_log

#endif 