#include "i3070/utils/SafeConversion.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace core {

bool show_conversion_debug = false;

namespace {
bool isShowConversion() {
    return show_conversion_debug;
}
}

int safeStoi(const string& str, const string& field_name, 
             const string& record_type, int default_value) {
    if (isShowConversion()) {
        cerr << "[DEBUG] safeStoi called for " << field_name << " in " << record_type 
                  << " with value: '" << str << "'" << endl;
    }
    if (str.empty()) {
        if (isShowConversion()) {
            cerr << "Error: Empty string for " << field_name 
                      << " in " << record_type << " record" << endl;
        }
        throw invalid_argument("Empty string for " + field_name + " in " + record_type + " record");
    }
    try {
        return stoi(str);
    } catch (const invalid_argument& e) {
        if (isShowConversion()) {
            cerr << "Error: Invalid argument for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        throw;
    } catch (const out_of_range& e) {
        if (isShowConversion()) {
            cerr << "Error: Out of range for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        throw;
    }
}

double safeStod(const string& str, const string& field_name, 
                const string& record_type, double default_value) {
    if (str.empty()) {
        if (isShowConversion()) {
            cerr << "Error: Empty string for " << field_name 
                      << " in " << record_type << " record" << endl;
        }
        throw invalid_argument("Empty string for " + field_name + " in " + record_type + " record");
    }
    try {
        return stod(str);
    } catch (const invalid_argument& e) {
        if (isShowConversion()) {
            cerr << "Error: Invalid argument for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        throw;
    } catch (const out_of_range& e) {
        if (isShowConversion()) {
            cerr << "Error: Out of range for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        throw;
    }
}

bool safeStob(const string& str, const string& field_name, 
              const string& record_type, bool default_value) {
    if (str.empty()) {
        if (isShowConversion()) {
            cerr << "Warning: Empty string for " << field_name 
                      << " in " << record_type << " record, using default: " << default_value << endl;
        }
        return default_value;
    }
    if (str == "1" || str == "true" || str == "TRUE" || str == "True" || 
        str == "Y" || str == "y" || str == "yes" || str == "YES") {
        return true;
    } else if (str == "0" || str == "false" || str == "FALSE" || str == "False" || 
               str == "N" || str == "n" || str == "no" || str == "NO") {
        return false;
    } else {
        if (isShowConversion()) {
            cerr << "Error: Invalid boolean value for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', using default: " << default_value << endl;
        }
        return default_value;
    }
}

optional<int> safeStoiOptional(const string& str, const string& field_name, 
                                   const string& record_type) {
    if (isShowConversion()) {
        cerr << "[DEBUG] safeStoiOptional called for " << field_name << " in " << record_type 
                  << " with value: '" << str << "'" << endl;
    }
    if (str.empty()) {
        if (isShowConversion()) {
            cerr << "Info: Empty string for " << field_name 
                      << " in " << record_type << " record, returning nullopt" << endl;
        }
        return nullopt;
    }
    try {
        return stoi(str);
    } catch (const invalid_argument& e) {
        if (isShowConversion()) {
            cerr << "Error: Invalid argument for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        return nullopt;
    } catch (const out_of_range& e) {
        if (isShowConversion()) {
            cerr << "Error: Out of range for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        return nullopt;
    }
}

optional<double> safeStodOptional(const string& str, const string& field_name, 
                                      const string& record_type) {
    if (isShowConversion()) {
        cerr << "[DEBUG] safeStodOptional called for " << field_name << " in " << record_type 
                  << " with value: '" << str << "'" << endl;
    }
    if (str.empty()) {
        if (isShowConversion()) {
            cerr << "Info: Empty string for " << field_name 
                      << " in " << record_type << " record, returning nullopt" << endl;
        }
        return nullopt;
    }
    try {
        return stod(str);
    } catch (const invalid_argument& e) {
        if (isShowConversion()) {
            cerr << "Error: Invalid argument for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        return nullopt;
    } catch (const out_of_range& e) {
        if (isShowConversion()) {
            cerr << "Error: Out of range for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        return nullopt;
    }
}

uint64_t safeStoull(const string& str, const string& field_name, 
                    const string& record_type, uint64_t default_value){
    if (isShowConversion()) {
        cerr << "[DEBUG] safeStoi called for " << field_name << " in " << record_type 
                  << " with value: '" << str << "'" << endl;
    }
    if (str.empty()) {
        if (isShowConversion()) {
            cerr << "Error: Empty string for " << field_name 
                      << " in " << record_type << " record" << endl;
        }
        throw invalid_argument("Empty string for " + field_name + " in " + record_type + " record");
    }
    try {
        return stoull(str);
    } catch (const invalid_argument& e) {
        if (isShowConversion()) {
            cerr << "Error: Invalid argument for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        throw;
    } catch (const out_of_range& e) {
        if (isShowConversion()) {
            cerr << "Error: Out of range for " << field_name 
                      << " in " << record_type << " record. Value: '" << str 
                      << "', Error: " << e.what() << endl;
        }
        throw;
    }
}

} // namespace core
} // namespace i3070 
