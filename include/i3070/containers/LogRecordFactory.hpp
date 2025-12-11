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

#pragma once

#include "../core/LogRecord.hpp"
#include "../records/AnalogRecords.hpp"
#include "../records/DigitalRecords.hpp"
#include "../records/SystemRecords.hpp"
#include "../records/ShortRecords.hpp"
#include "../enums/LogRecordPrefix.hpp"
#include <memory>
#include <string>

namespace i3070::containers {

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
    inline static std::unique_ptr<core::LogRecord> createRecord(enums::LogRecordPrefix prefix) {
        using namespace enums;
        using namespace records;
        switch (prefix) {
            // Analog Test Records
            case LogRecordPrefix::A_CAP: return std::make_unique<records::CapacitorRecord>();
            case LogRecordPrefix::A_DIO: return std::make_unique<records::DiodeRecord>();
            case LogRecordPrefix::A_FUS: return std::make_unique<records::FuseRecord>();
            case LogRecordPrefix::A_IND: return std::make_unique<records::InductorRecord>();
            case LogRecordPrefix::A_JUM: return std::make_unique<records::JumperRecord>();
            case LogRecordPrefix::A_MEA: return std::make_unique<records::MeasureRecord>();
            case LogRecordPrefix::A_NFE: return std::make_unique<records::NFetRecord>();
            case LogRecordPrefix::A_NPN: return std::make_unique<records::NpnTransistorRecord>();
            case LogRecordPrefix::A_PFE: return std::make_unique<records::PFetRecord>();
            case LogRecordPrefix::A_PNP: return std::make_unique<records::PnpTransistorRecord>();
            case LogRecordPrefix::A_POT: return std::make_unique<records::PotentiometerRecord>();
            case LogRecordPrefix::A_RES: return std::make_unique<records::ResistorRecord>();
            case LogRecordPrefix::A_SWI: return std::make_unique<records::SwitchRecord>();
            case LogRecordPrefix::A_ZEN: return std::make_unique<records::ZenerDiodeRecord>();
            // Limit Records
            case LogRecordPrefix::LIM2: return std::make_unique<records::Lim2Record>();
            case LogRecordPrefix::LIM3: return std::make_unique<records::Lim3Record>();
            // System & Batch Records
            case LogRecordPrefix::BATCH: return std::make_unique<records::BatchRecord>();
            case LogRecordPrefix::BLOCK: return std::make_unique<records::BlockRecord>();
            case LogRecordPrefix::ALM:   return std::make_unique<records::AlarmRecord>();
            case LogRecordPrefix::AID:   return std::make_unique<records::AlarmBoardRecord>();
            case LogRecordPrefix::ARRAY: return std::make_unique<records::ArrayRecord>();
            case LogRecordPrefix::NETV:  return std::make_unique<records::NetVerifyRecord>();
            case LogRecordPrefix::NODE:  return std::make_unique<records::NodeListRecord>();
            case LogRecordPrefix::RPT:   return std::make_unique<records::ReportRecord>();
            case LogRecordPrefix::RETEST:return std::make_unique<records::RetestRecord>();
            // Digital Test Records
            case LogRecordPrefix::D_T:   return std::make_unique<records::DigitalTestRecord>();
            case LogRecordPrefix::DPIN:  return std::make_unique<records::DevicePinRecord>();
            case LogRecordPrefix::D_PLD: return std::make_unique<records::PLDProgrammingRecord>();
            case LogRecordPrefix::CCHK:  return std::make_unique<records::ConnectCheckRecord>();
            case LogRecordPrefix::PCHK:  return std::make_unique<records::PolarityCheckRecord>();
            case LogRecordPrefix::TJET:  return std::make_unique<records::TestJetRecord>();
            case LogRecordPrefix::INDICT:return std::make_unique<records::IndictmentRecord>();
            case LogRecordPrefix::PIN:   return std::make_unique<records::PinRecord>();
            // Shorts & Opens Records
            case LogRecordPrefix::TS:    return std::make_unique<records::ShortsTestRecord>();
            case LogRecordPrefix::TS_S:  return std::make_unique<records::TsSourceRecord>();
            case LogRecordPrefix::TS_D:  return std::make_unique<records::TsDestinationRecord>();
            case LogRecordPrefix::TS_O:  return std::make_unique<records::TsOpenRecord>();
            case LogRecordPrefix::TS_P:  return std::make_unique<records::TsPhantomRecord>();
            // Boundary Scan Records
            case LogRecordPrefix::BS_CON:return std::make_unique<records::BoundaryScanRecord>();
            case LogRecordPrefix::BS_O:  return std::make_unique<records::BsOpenRecord>();
            case LogRecordPrefix::BS_S:  return std::make_unique<records::BsShortRecord>();
            case LogRecordPrefix::BTEST: return std::make_unique<records::BTestRecord>();
            case LogRecordPrefix::PF:    return std::make_unique<records::PFRecord>();
            // Default fallback
            default: return std::make_unique<core::LogRecord>();
        }
    }

    /**
     * @brief Convert string prefix to enum
     */
    inline static enums::LogRecordPrefix stringToPrefix(const std::string& str) {
        return enums::stringToPrefix(str);
    }
};

} // namespace i3070::containers
