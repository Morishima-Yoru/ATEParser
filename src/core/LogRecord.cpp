#include "i3070/core/LogRecord.hpp"
#include "i3070/utils/ConfigReader.hpp"
#include "i3070/enums/LogRecordPrefix.hpp"
#include <set>
#include <mutex>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace core {

namespace {
// Singleton ConfigReader, initialized only once
ConfigReader& getConfig() {
    static ConfigReader config("config.ini");
    return config;
}

bool isShowUnimplementedPrefix() {
    static bool cached = false;
    static once_flag flag;
    call_once(flag, []() {
        cached = getConfig().getBool("DEBUG", "SHOW_UNIMPLEMENTED_PREFIX", false);
    });
    return cached;
}
}

bool LogRecord::show_raw_field = false;

string LogRecord::prefixToString(enums::LogRecordPrefix prefix) {
    // Directly call prefixToString in enums
    return i3070::enums::prefixToString(prefix);
}

void LogRecord::fromFields(const vector<string>& fields) {
    if (!isShowUnimplementedPrefix()) {return;}
    static set<enums::LogRecordPrefix> printed;
    if (printed.find(prefix) == printed.end()) {
        string prefixToShow = raw_prefix.empty() ? prefixToString(prefix) : raw_prefix;
        printf("{%s} UNIMPLEMENTED!\n", prefixToShow.c_str());
    }
    printed.insert(prefix);
}

json LogRecord::toJson() const {
    json j;
    j["prefix"] = prefixToString(prefix);
    // If prefix is UNKNOWN, always output raw
    if (prefix == enums::LogRecordPrefix::UNKNOWN || show_raw_field) j["raw"] = raw_data;
    return j;
}

} // namespace core
} // namespace i3070 