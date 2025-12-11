#ifndef I3070_CORE_CONFIGREADER_HPP
#define I3070_CORE_CONFIGREADER_HPP

#include <string>
#include <unordered_map>

using namespace std;

namespace i3070 {
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
} // namespace i3070

#endif 