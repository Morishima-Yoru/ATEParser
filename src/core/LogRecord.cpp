#include "i3070/core/LogRecord.hpp"
#include "i3070/enums/LogRecordPrefix.hpp"
#include <set>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace core {

bool LogRecord::show_raw_field = false;
bool LogRecord::show_unimplemented_prefix = false;

string LogRecord::prefixToString(enums::LogRecordPrefix prefix) {
    // Directly call prefixToString in enums
    return i3070::enums::prefixToString(prefix);
}

void LogRecord::fromFields(const vector<string>& fields) {
    if (!show_unimplemented_prefix) {return;}
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