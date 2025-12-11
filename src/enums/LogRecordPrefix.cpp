/**
 * @file LogRecordPrefix.cpp
 * @brief Implements string↔enum conversions and utilities for log prefixes
 *
 * This source defines:
 *  - stringToPrefix(): parse "@PREFIX" strings into enum values
 *  - prefixToString(): format enum values back to string prefixes
 *  - isAnalogTestPrefix(), isDigitalTestPrefix(), isShortsTestPrefix(), isSystemRecordPrefix()
 *  - getPrefixDescription(): human-readable explanations
 *  - getPrefixCategory(): logical grouping names
 *
 * Based on Keysight i3070 Log Record Format (Table 8-1) [1].
 */

#include "i3070/enums/LogRecordPrefix.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace enums {

// Mapping from string to enum
LogRecordPrefix stringToPrefix(const string& prefix_str) {
    static const unordered_map<string, LogRecordPrefix> map = {
        // Analog test prefixes
        {"@A-CAP", LogRecordPrefix::A_CAP},
        {"@A-DIO", LogRecordPrefix::A_DIO},
        {"@A-FUS", LogRecordPrefix::A_FUS},
        {"@A-IND", LogRecordPrefix::A_IND},
        {"@A-JUM", LogRecordPrefix::A_JUM},
        {"@A-MEA", LogRecordPrefix::A_MEA},
        {"@A-NFE", LogRecordPrefix::A_NFE},
        {"@A-NPN", LogRecordPrefix::A_NPN},
        {"@A-PFE", LogRecordPrefix::A_PFE},
        {"@A-PNP", LogRecordPrefix::A_PNP},
        {"@A-POT", LogRecordPrefix::A_POT},
        {"@A-RES", LogRecordPrefix::A_RES},
        {"@A-SWI", LogRecordPrefix::A_SWI},
        {"@A-ZEN", LogRecordPrefix::A_ZEN},
        // System & batch prefixes
        {"@ALM",    LogRecordPrefix::ALM},
        {"@AID",    LogRecordPrefix::AID},
        {"@ARRAY",  LogRecordPrefix::ARRAY},
        {"@BATCH",  LogRecordPrefix::BATCH},
        {"@BLOCK",  LogRecordPrefix::BLOCK},
        // Boundary-scan prefixes
        {"@BS-CON", LogRecordPrefix::BS_CON},
        {"@BS-O",   LogRecordPrefix::BS_O},
        {"@BS-S",   LogRecordPrefix::BS_S},
        {"@BTEST",  LogRecordPrefix::BTEST},
        // Digital prefixes
        {"@CCHK",   LogRecordPrefix::CCHK},
        {"@DPIN",   LogRecordPrefix::DPIN},
        {"@D-PLD",  LogRecordPrefix::D_PLD},
        {"@D-T",    LogRecordPrefix::D_T},
        // Shorts & opens prefixes
        {"@TS",     LogRecordPrefix::TS},
        {"@TS-S",   LogRecordPrefix::TS_S},
        {"@TS-D",   LogRecordPrefix::TS_D},
        {"@TS-O",   LogRecordPrefix::TS_O},
        {"@TS-P",   LogRecordPrefix::TS_P},
        // Diagnostic & analysis prefixes
        {"@INDICT", LogRecordPrefix::INDICT},
        {"@NETV",   LogRecordPrefix::NETV},
        {"@NODE",   LogRecordPrefix::NODE},
        {"@PCHK",   LogRecordPrefix::PCHK},
        {"@PIN",    LogRecordPrefix::PIN},
        {"@PF",     LogRecordPrefix::PF},
        {"@PRB",    LogRecordPrefix::PRB},
        {"@RETEST", LogRecordPrefix::RETEST},
        {"@RPT",    LogRecordPrefix::RPT},
        {"@TJET",   LogRecordPrefix::TJET},
        // Limit & specification prefixes
        {"@LIM2",   LogRecordPrefix::LIM2},
        {"@LIM3",   LogRecordPrefix::LIM3}
    };
    auto it = map.find(prefix_str);
    return (it != map.end() ? it->second : LogRecordPrefix::UNKNOWN);
}

// Mapping from enum to string
string prefixToString(LogRecordPrefix prefix) {
    switch (prefix) {
        case LogRecordPrefix::A_CAP: return "@A-CAP";
        case LogRecordPrefix::A_DIO: return "@A-DIO";
        case LogRecordPrefix::A_FUS: return "@A-FUS";
        case LogRecordPrefix::A_IND: return "@A-IND";
        case LogRecordPrefix::A_JUM: return "@A-JUM";
        case LogRecordPrefix::A_MEA: return "@A-MEA";
        case LogRecordPrefix::A_NFE: return "@A-NFE";
        case LogRecordPrefix::A_NPN: return "@A-NPN";
        case LogRecordPrefix::A_PFE: return "@A-PFE";
        case LogRecordPrefix::A_PNP: return "@A-PNP";
        case LogRecordPrefix::A_POT: return "@A-POT";
        case LogRecordPrefix::A_RES: return "@A-RES";
        case LogRecordPrefix::A_SWI: return "@A-SWI";
        case LogRecordPrefix::A_ZEN: return "@A-ZEN";
        case LogRecordPrefix::ALM:   return "@ALM";
        case LogRecordPrefix::AID:   return "@AID";
        case LogRecordPrefix::ARRAY: return "@ARRAY";
        case LogRecordPrefix::BATCH: return "@BATCH";
        case LogRecordPrefix::BLOCK: return "@BLOCK";
        case LogRecordPrefix::BS_CON:return "@BS-CON";
        case LogRecordPrefix::BS_O:  return "@BS-O";
        case LogRecordPrefix::BS_S:  return "@BS-S";
        case LogRecordPrefix::BTEST: return "@BTEST";
        case LogRecordPrefix::CCHK:  return "@CCHK";
        case LogRecordPrefix::DPIN:  return "@DPIN";
        case LogRecordPrefix::D_PLD: return "@D-PLD";
        case LogRecordPrefix::D_T:   return "@D-T";
        case LogRecordPrefix::TS:    return "@TS";
        case LogRecordPrefix::TS_S:  return "@TS-S";
        case LogRecordPrefix::TS_D:  return "@TS-D";
        case LogRecordPrefix::TS_O:  return "@TS-O";
        case LogRecordPrefix::TS_P:  return "@TS-P";
        case LogRecordPrefix::INDICT:return "@INDICT";
        case LogRecordPrefix::NETV:  return "@NETV";
        case LogRecordPrefix::NODE:  return "@NODE";
        case LogRecordPrefix::PCHK:  return "@PCHK";
        case LogRecordPrefix::PIN:   return "@PIN";
        case LogRecordPrefix::PF:    return "@PF";
        case LogRecordPrefix::PRB:   return "@PRB";
        case LogRecordPrefix::RETEST:return "@RETEST";
        case LogRecordPrefix::RPT:   return "@RPT";
        case LogRecordPrefix::TJET:  return "@TJET";
        case LogRecordPrefix::LIM2:  return "@LIM2";
        case LogRecordPrefix::LIM3:  return "@LIM3";
        default:                     return "@UNKNOWN";
    }
}

// Category checks

bool isAnalogTestPrefix(LogRecordPrefix prefix) {
    return (prefix >= LogRecordPrefix::A_CAP && prefix <= LogRecordPrefix::A_ZEN);
}

bool isDigitalTestPrefix(LogRecordPrefix prefix) {
    return (prefix == LogRecordPrefix::CCHK ||
            prefix == LogRecordPrefix::DPIN ||
            prefix == LogRecordPrefix::D_PLD ||
            prefix == LogRecordPrefix::D_T);
}

bool isShortsTestPrefix(LogRecordPrefix prefix) {
    return (prefix >= LogRecordPrefix::TS && prefix <= LogRecordPrefix::TS_P);
}

bool isSystemRecordPrefix(LogRecordPrefix prefix) {
    return (prefix == LogRecordPrefix::ALM   ||
            prefix == LogRecordPrefix::AID   ||
            prefix == LogRecordPrefix::ARRAY ||
            prefix == LogRecordPrefix::BATCH ||
            prefix == LogRecordPrefix::BLOCK);
}

// Descriptions and categories

string getPrefixDescription(LogRecordPrefix prefix) {
    switch (prefix) {
        // Analog
        case LogRecordPrefix::A_CAP:   return "Capacitor test result record";  
        case LogRecordPrefix::A_DIO:   return "Diode test result record";      
        case LogRecordPrefix::A_FUS:   return "Fuse test result record";       
        case LogRecordPrefix::A_IND:   return "Inductor test result record";   
        case LogRecordPrefix::A_JUM:   return "Jumper test result record";     
        case LogRecordPrefix::A_MEA:   return "Measurement result record";     
        case LogRecordPrefix::A_NFE:   return "N-channel FET test result record";  
        case LogRecordPrefix::A_NPN:   return "NPN transistor test result record";   
        case LogRecordPrefix::A_PFE:   return "P-channel FET test result record";  
        case LogRecordPrefix::A_PNP:   return "PNP transistor test result record";  
        case LogRecordPrefix::A_POT:   return "Potentiometer test result record";  
        case LogRecordPrefix::A_RES:   return "Resistor test result record";    
        case LogRecordPrefix::A_SWI:   return "Switch test result record";      
        case LogRecordPrefix::A_ZEN:   return "Zener diode test result record";  
        // System & Batch
        case LogRecordPrefix::ALM:     return "Real-time alarm identification record";  
        case LogRecordPrefix::AID:     return "Alarm-inducing board identification record";  
        case LogRecordPrefix::ARRAY:   return "Digitizer result analysis record";  
        case LogRecordPrefix::BATCH:   return "Batch identification and configuration record";  
        case LogRecordPrefix::BLOCK:   return "Test block identification record";  
        // Boundary-scan
        case LogRecordPrefix::BS_CON:  return "Boundary scan test description record";  
        case LogRecordPrefix::BS_O:    return "Boundary scan open pins list record";  
        case LogRecordPrefix::BS_S:    return "Boundary scan short pins list record";  
        case LogRecordPrefix::BTEST:   return "Board test description and results record";  
        // Digital
        case LogRecordPrefix::CCHK:    return "ConnectCheck test result record";  
        case LogRecordPrefix::DPIN:    return "Device pin list record";  
        case LogRecordPrefix::D_PLD:   return "PLD programming result record";  
        case LogRecordPrefix::D_T:     return "Digital test result record";  
        // Shorts & Opens
        case LogRecordPrefix::TS:      return "Shorts test result record (main)";  
        case LogRecordPrefix::TS_S:    return "Shorts test source record";  
        case LogRecordPrefix::TS_D:    return "Shorts test destination record";  
        case LogRecordPrefix::TS_O:    return "Opens test result record";  
        case LogRecordPrefix::TS_P:    return "Phantom shorts list record";  
        // Diagnostic & Analysis
        case LogRecordPrefix::INDICT:  return "Indicated faulty devices record";  
        case LogRecordPrefix::NETV:    return "Network verification record";  
        case LogRecordPrefix::NODE:    return "Node list record";  
        case LogRecordPrefix::PCHK:    return "Polarity check test result record";  
        case LogRecordPrefix::PIN:     return "Pin list record";  
        case LogRecordPrefix::PF:      return "Pin fault result record";  
        case LogRecordPrefix::PRB:     return "Probe fault result record";  
        case LogRecordPrefix::RETEST:  return "Retest clear indication record";  
        case LogRecordPrefix::RPT:     return "Report record message";  
        case LogRecordPrefix::TJET:    return "VTEP/TestJet result record";  
        // Limits
        case LogRecordPrefix::LIM2:    return "Analog test high/low limits record";  
        case LogRecordPrefix::LIM3:    return "Analog test nominal and tolerance limits record";  
        default:                       return "Unknown or custom prefix type";  
    }
}

string getPrefixCategory(LogRecordPrefix prefix) {
    if (isAnalogTestPrefix(prefix))        return "Analog Test";
    if (isDigitalTestPrefix(prefix))       return "Digital Test";
    if (isShortsTestPrefix(prefix))        return "Shorts/Opens Test";
    if (isSystemRecordPrefix(prefix))      return "System/Batch";
    switch (prefix) {
        case LogRecordPrefix::BS_CON:
        case LogRecordPrefix::BS_O:
        case LogRecordPrefix::BS_S:
        case LogRecordPrefix::BTEST:
            return "Boundary Scan";
        case LogRecordPrefix::INDICT:
        case LogRecordPrefix::NETV:
        case LogRecordPrefix::NODE:
        case LogRecordPrefix::PCHK:
        case LogRecordPrefix::PIN:
        case LogRecordPrefix::PF:
        case LogRecordPrefix::PRB:
        case LogRecordPrefix::RETEST:
        case LogRecordPrefix::RPT:
        case LogRecordPrefix::TJET:
            return "Diagnostic/Analysis";
        case LogRecordPrefix::LIM2:
        case LogRecordPrefix::LIM3:
            return "Limits/Specification";
        default:
            return "Unknown";
    }
}

} // namespace enums
} // namespace i3070
