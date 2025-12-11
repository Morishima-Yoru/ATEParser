/**
 * @file LogRecordFactory.hpp
 * @brief Factory for creating LogRecord instances by prefix
 *
 * This header defines a factory class that constructs appropriate 
 * LogRecord-derived objects based on the provided LogRecordPrefix.
 * It also provides utilities to convert between string prefixes and
 * enum values, ensuring strict conformance to the i3070 log format.
 *
 * Based on Keysight i3070 Log Record Format (Section 8-3) and 
 * Hierarchy of Log Records (Section 8-6).
 *
 * @date 2025-06-19
 */

#ifndef I3070_CONTAINERS_LOGRECORDFACTORY_HPP
#define I3070_CONTAINERS_LOGRECORDFACTORY_HPP

#include "../core/LogRecord.hpp"
#include "../records/AnalogRecords.hpp"
#include "../records/DigitalRecords.hpp"
#include "../records/SystemRecords.hpp"
#include "../records/ShortRecords.hpp"
#include "../enums/LogRecordPrefix.hpp"
#include <memory>
#include <string>
#include <unordered_map>

using namespace std;

namespace i3070 {
namespace containers {

/**
 * @class LogRecordFactory
 * @brief Creates specific LogRecord objects from a prefix identifier
 *
 * This factory leverages the LogRecordPrefix enum to instantiate the
 * correct subclass of LogRecord for parsing and storing log data.
 */
class LogRecordFactory {
public:
    /**
     * @brief Create a LogRecord instance corresponding to the given prefix
     */
    inline static unique_ptr<core::LogRecord> createRecord(enums::LogRecordPrefix prefix) {
        using namespace enums;
        using namespace records;
        switch (prefix) {
            // Analog Test Records
            case LogRecordPrefix::A_CAP: return make_unique<records::CapacitorRecord>();
            case LogRecordPrefix::A_DIO: return make_unique<records::DiodeRecord>();
            case LogRecordPrefix::A_FUS: return make_unique<records::FuseRecord>();
            case LogRecordPrefix::A_IND: return make_unique<records::InductorRecord>();
            case LogRecordPrefix::A_JUM: return make_unique<records::JumperRecord>();
            case LogRecordPrefix::A_MEA: return make_unique<records::MeasureRecord>();
            case LogRecordPrefix::A_NFE: return make_unique<records::NFetRecord>();
            case LogRecordPrefix::A_NPN: return make_unique<records::NpnTransistorRecord>();
            case LogRecordPrefix::A_PFE: return make_unique<records::PFetRecord>();
            case LogRecordPrefix::A_PNP: return make_unique<records::PnpTransistorRecord>();
            case LogRecordPrefix::A_POT: return make_unique<records::PotentiometerRecord>();
            case LogRecordPrefix::A_RES: return make_unique<records::ResistorRecord>();
            case LogRecordPrefix::A_SWI: return make_unique<records::SwitchRecord>();
            case LogRecordPrefix::A_ZEN: return make_unique<records::ZenerDiodeRecord>();
            // Limit Records
            case LogRecordPrefix::LIM2: return make_unique<records::Lim2Record>();
            case LogRecordPrefix::LIM3: return make_unique<records::Lim3Record>();
            // System & Batch Records
            case LogRecordPrefix::BATCH: return make_unique<records::BatchRecord>();
            case LogRecordPrefix::BLOCK: return make_unique<records::BlockRecord>();
            case LogRecordPrefix::ALM:   return make_unique<records::AlarmRecord>();
            case LogRecordPrefix::AID:   return make_unique<records::AlarmBoardRecord>();
            case LogRecordPrefix::ARRAY: return make_unique<records::ArrayRecord>();
            case LogRecordPrefix::NETV:  return make_unique<records::NetVerifyRecord>();
            case LogRecordPrefix::NODE:  return make_unique<records::NodeListRecord>();
            case LogRecordPrefix::RPT:   return make_unique<records::ReportRecord>();
            case LogRecordPrefix::RETEST:return make_unique<records::RetestRecord>();
            // Digital Test Records
            case LogRecordPrefix::D_T:   return make_unique<records::DigitalTestRecord>();
            case LogRecordPrefix::DPIN:  return make_unique<records::DevicePinRecord>();
            case LogRecordPrefix::D_PLD: return make_unique<records::PLDProgrammingRecord>();
            case LogRecordPrefix::CCHK:  return make_unique<records::ConnectCheckRecord>();
            case LogRecordPrefix::PCHK:  return make_unique<records::PolarityCheckRecord>();
            case LogRecordPrefix::TJET:  return make_unique<records::TestJetRecord>();
            case LogRecordPrefix::INDICT:return make_unique<records::IndictmentRecord>();
            case LogRecordPrefix::PIN:   return make_unique<records::PinRecord>();
            // Shorts & Opens Records
            case LogRecordPrefix::TS:    return make_unique<records::ShortsTestRecord>();
            case LogRecordPrefix::TS_S:  return make_unique<records::TsSourceRecord>();
            case LogRecordPrefix::TS_D:  return make_unique<records::TsDestinationRecord>();
            case LogRecordPrefix::TS_O:  return make_unique<records::TsOpenRecord>();
            case LogRecordPrefix::TS_P:  return make_unique<records::TsPhantomRecord>();
            // Boundary Scan Records
            case LogRecordPrefix::BS_CON:return make_unique<records::BoundaryScanRecord>();
            case LogRecordPrefix::BS_O:  return make_unique<records::BsOpenRecord>();
            case LogRecordPrefix::BS_S:  return make_unique<records::BsShortRecord>();
            case LogRecordPrefix::BTEST: return make_unique<records::BTestRecord>();
            case LogRecordPrefix::PF:    return make_unique<records::PFRecord>();
            // Default fallback
            default: return make_unique<core::LogRecord>();
        }
    }

    /**
     * @brief Convert a string prefix to a LogRecordPrefix enum
     */
    inline static enums::LogRecordPrefix stringToPrefix(const string& prefixStr) {
        using i3070::enums::LogRecordPrefix;
        using namespace i3070::enums;
        static const unordered_map<string, LogRecordPrefix> map = {
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
            {"@LIM2", LogRecordPrefix::LIM2},
            {"@LIM3", LogRecordPrefix::LIM3},
            {"@BATCH", LogRecordPrefix::BATCH},
            {"@BLOCK", LogRecordPrefix::BLOCK},
            {"@ALM", LogRecordPrefix::ALM},
            {"@AID", LogRecordPrefix::AID},
            {"@ARRAY", LogRecordPrefix::ARRAY},
            {"@NETV", LogRecordPrefix::NETV},
            {"@NODE", LogRecordPrefix::NODE},
            {"@RPT", LogRecordPrefix::RPT},
            {"@RETEST", LogRecordPrefix::RETEST},
            {"@D-T", LogRecordPrefix::D_T},
            {"@DPIN", LogRecordPrefix::DPIN},
            {"@D-PLD", LogRecordPrefix::D_PLD},
            {"@CCHK", LogRecordPrefix::CCHK},
            {"@PCHK", LogRecordPrefix::PCHK},
            {"@TJET", LogRecordPrefix::TJET},
            {"@INDICT", LogRecordPrefix::INDICT},
            {"@PIN", LogRecordPrefix::PIN},
            {"@TS", LogRecordPrefix::TS},
            {"@TS-S", LogRecordPrefix::TS_S},
            {"@TS-D", LogRecordPrefix::TS_D},
            {"@TS-O", LogRecordPrefix::TS_O},
            {"@TS-P", LogRecordPrefix::TS_P},
            {"@BS-CON", LogRecordPrefix::BS_CON},
            {"@BS-O", LogRecordPrefix::BS_O},
            {"@BS-S", LogRecordPrefix::BS_S},
            {"@BTEST", LogRecordPrefix::BTEST},
            {"@PF", LogRecordPrefix::PF},
        };
        auto it = map.find(prefixStr);
        return (it != map.end() ? it->second : LogRecordPrefix::UNKNOWN);
    }
};

} // namespace containers
} // namespace i3070

#endif // I3070_CONTAINERS_LOGRECORDFACTORY_HPP
