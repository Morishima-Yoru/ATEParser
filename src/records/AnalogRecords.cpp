/**
 * @file AnalogRecords.cpp
 * @brief Implements analog test record parsing and JSON serialization
 *
 * This source file defines:
 *  - Constructors for each analog record type
 *  - toJson() to convert records to JSON format
 *
 * Based strictly on Keysight_i3070_LogRecord_Format.pdf, Tables 8-4 to 8-11 [1].
 */

#include "i3070/records/AnalogRecords.hpp"
#include "i3070/utils/JsonKeys.hpp"
#include "i3070/utils/SafeConversion.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <cmath>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace records {


// ========= Common Base: AnalogTestRecord =========

AnalogTestRecord::AnalogTestRecord(enums::LogRecordPrefix prefix_type)
  : LogRecord(prefix_type),
    test_status(enums::AnalogTestStatus::PASSED),
    measured_value(nullopt),
    subtest_designator(nullopt),
    limit_record(nullptr) {}

bool AnalogTestRecord::isPassed() const {
    return test_status == enums::AnalogTestStatus::PASSED;
}
bool AnalogTestRecord::isFailed() const {
    return test_status != enums::AnalogTestStatus::PASSED;
}
bool AnalogTestRecord::isAborted() const {
    return test_status == enums::AnalogTestStatus::ABORTED_BY_OPERATOR;
}
string AnalogTestRecord::getStatusDescription() const {
    // Example mapping; real descriptions per spec Table 8-4
    switch (test_status) {
        case enums::AnalogTestStatus::PASSED: return "Passed";
        case enums::AnalogTestStatus::FAILED: return "Failed";
        case enums::AnalogTestStatus::FAILED_COMPLIANCE_LIMIT: return "Compliance Limit"; 
        case enums::AnalogTestStatus::FAILED_DETECTOR_TIMEOUT: return "Detector Timeout"; 
        default: return "Unknown Status"; 
    }
}
bool AnalogTestRecord::hasMeasuredValue() const {
    return measured_value.has_value();
}
double AnalogTestRecord::getMeasuredValue() const {
    return measured_value.value_or(0.0);
}
bool AnalogTestRecord::hasSubtestDesignator() const {
    return subtest_designator.has_value();
}
string AnalogTestRecord::getSubtestDesignator() const {
    return subtest_designator.value_or("");
}
bool AnalogTestRecord::hasLimitRecord() const {
    return limit_record != nullptr;
}
LimitRecord* AnalogTestRecord::getLimitRecord() const {
    return limit_record.get();
}
void AnalogTestRecord::setLimitRecord(unique_ptr<LimitRecord> limit_rec) {
    limit_record = std::move(limit_rec);
}

// --------------------------- Lim2Record ---------------------------

Lim2Record::Lim2Record()
    : LimitRecord(i3070::enums::LogRecordPrefix::LIM2),
      high_limit(0.0),
      low_limit(0.0) {}

Lim2Record::Lim2Record(double high_limit_val, double low_limit_val)
    : LimitRecord(i3070::enums::LogRecordPrefix::LIM2),
      high_limit(high_limit_val),
      low_limit(low_limit_val) {}

string Lim2Record::getLimitType() const {
    return "LIM2";
}

bool Lim2Record::hasNominalValue() const {
    return false;
}

double Lim2Record::getHighLimit() const {
    return high_limit;
}

double Lim2Record::getLowLimit() const {
    return low_limit;
}

bool Lim2Record::isWithinLimits(double measured_value) const {
    return (measured_value >= low_limit) && (measured_value <= high_limit);
}

double Lim2Record::getRange() const {
    return high_limit - low_limit;
}

double Lim2Record::getCenterPoint() const {
    return (high_limit + low_limit) / 2.0;
}

// --------------------------- Lim3Record ---------------------------

Lim3Record::Lim3Record()
    : LimitRecord(i3070::enums::LogRecordPrefix::LIM3),
      nominal_value(0.0),
      high_limit(0.0),
      low_limit(0.0) {}

Lim3Record::Lim3Record(double nominal_val,
                       double high_limit_val,
                       double low_limit_val)
    : LimitRecord(i3070::enums::LogRecordPrefix::LIM3),
      nominal_value(nominal_val),
      high_limit(high_limit_val),
      low_limit(low_limit_val) {}

string Lim3Record::getLimitType() const {
    return "LIM3";
}

bool Lim3Record::hasNominalValue() const {
    return true;
}

double Lim3Record::getNominalValue() const {
    return nominal_value;
}

double Lim3Record::getHighLimit() const {
    return high_limit;
}

double Lim3Record::getLowLimit() const {
    return low_limit;
}

bool Lim3Record::isWithinLimits(double measured_value) const {
    return (measured_value >= low_limit) && (measured_value <= high_limit);
}

double Lim3Record::getPositiveTolerance() const {
    return high_limit - nominal_value;
}

double Lim3Record::getNegativeTolerance() const {
    return nominal_value - low_limit;
}

double Lim3Record::getPercentageDeviation(double measured_value) const {
    if (nominal_value == 0.0) {
        throw domain_error("Nominal value is zero, cannot compute percentage deviation");
    }
    return ((measured_value - nominal_value) / nominal_value) * 100.0;
}

bool Lim3Record::hasSymmetricTolerance() const {
    return fabs((high_limit - nominal_value) - (nominal_value - low_limit)) < 1e-9;
}

// ------------------------ Utility Functions ------------------------

unique_ptr<AnalogTestRecord>
createAnalogTestRecord(enums::LogRecordPrefix prefix) {
    using Prefix = enums::LogRecordPrefix;
    switch (prefix) {
        case Prefix::A_CAP: return make_unique<CapacitorRecord>();
        case Prefix::A_DIO: return make_unique<DiodeRecord>();
        case Prefix::A_FUS: return make_unique<FuseRecord>();
        case Prefix::A_IND: return make_unique<InductorRecord>();
        case Prefix::A_JUM: return make_unique<JumperRecord>();
        case Prefix::A_MEA: return make_unique<MeasureRecord>();
        case Prefix::A_NFE: return make_unique<NFetRecord>();
        case Prefix::A_NPN: return make_unique<NpnTransistorRecord>();
        case Prefix::A_PFE: return make_unique<PFetRecord>();
        case Prefix::A_PNP: return make_unique<PnpTransistorRecord>();
        case Prefix::A_POT: return make_unique<PotentiometerRecord>();
        case Prefix::A_RES: return make_unique<ResistorRecord>();
        case Prefix::A_SWI: return make_unique<SwitchRecord>();
        case Prefix::A_ZEN: return make_unique<ZenerDiodeRecord>();
        default:            return nullptr;
    }
}

bool isAnalogTestPrefix(enums::LogRecordPrefix prefix) {
    using Prefix = enums::LogRecordPrefix;
    return (prefix >= Prefix::A_CAP && prefix <= Prefix::A_ZEN);
}

bool expectsLim3Record(enums::LogRecordPrefix prefix) {
    using Prefix = enums::LogRecordPrefix;
    switch (prefix) {
        case Prefix::A_CAP:
        case Prefix::A_IND:
        case Prefix::A_POT:
        case Prefix::A_RES:
        case Prefix::A_ZEN:
            return true;
        case Prefix::A_DIO:
        case Prefix::A_FUS:
        case Prefix::A_JUM:
        case Prefix::A_MEA:
        case Prefix::A_NFE:
        case Prefix::A_NPN:
        case Prefix::A_PFE:
        case Prefix::A_PNP:
        case Prefix::A_SWI:
            return false;
        default:
            throw invalid_argument("Prefix is not an analog test type");
    }
}

enums::AnalogTestStatus parseAnalogTestStatus(int status_value) {
    using Status = enums::AnalogTestStatus;
    switch (status_value) {
        case 0:  return Status::PASSED;
        case 1:  return Status::FAILED;
        case 2:  return Status::FAILED_COMPLIANCE_LIMIT;
        case 3:  return Status::FAILED_DETECTOR_TIMEOUT;
        case 7:  return Status::FAILED_GENERAL;
        case 11: return Status::ABORTED_BY_OPERATOR;
        default: throw out_of_range("Unknown analog test status value");
    }
}

string analogTestStatusToString(enums::AnalogTestStatus status) {
    switch (status) {
        case enums::AnalogTestStatus::PASSED:                  return "Passed";
        case enums::AnalogTestStatus::FAILED:                  return "Failed";
        case enums::AnalogTestStatus::FAILED_COMPLIANCE_LIMIT: return "Compliance Limit";
        case enums::AnalogTestStatus::FAILED_DETECTOR_TIMEOUT: return "Detector Timeout";
        case enums::AnalogTestStatus::FAILED_GENERAL:          return "General Failure";
        case enums::AnalogTestStatus::ABORTED_BY_OPERATOR:     return "Aborted by Operator";
        default:                                               return "Unknown Status";
    }
}

bool validateMeasuredValue(double measured_value, const LimitRecord* limit_record) {
    if (!limit_record) {
        throw invalid_argument("Limit record pointer is null");
    }
    return limit_record->isWithinLimits(measured_value);
}

// ===================== LimitRecord =====================
void LimitRecord::fromFields(const vector<string>& fields) {}
json LimitRecord::toJson() const {
    json j = LogRecord::toJson();
    j[JSON_KEY_HIGH_LIMIT] = getHighLimit();
    j[JSON_KEY_LOW_LIMIT] = getLowLimit();
    return j;
}

// ===================== Lim2Record =====================
void Lim2Record::fromFields(const vector<string>& fields) {
    if (fields.size() > 0) high_limit = stod(fields[0]);
    if (fields.size() > 1) low_limit = stod(fields[1]);
}
json Lim2Record::toJson() const {
    return LimitRecord::toJson();
}

// ===================== Lim3Record =====================
void Lim3Record::fromFields(const vector<string>& fields) {
    // @LIM3|nominal|high|low
    if (fields.size() > 0) nominal_value = stod(fields[0]);
    if (fields.size() > 1) high_limit = stod(fields[1]);
    if (fields.size() > 2) low_limit = stod(fields[2]);
}
json Lim3Record::toJson() const {
    json j = LimitRecord::toJson();
    j[JSON_KEY_NOMINAL_VALUE] = getNominalValue();
    return j;
}

// ===================== AnalogTestRecord and subclasses =====================
void AnalogTestRecord::fromFields(const vector<string>& fields) {
    // Table 8-15: @A-RES, @A-CAP, @A-DIO, ... format
    if (fields.size() == 4) {
        if (fields.size() > 0) test_status = static_cast<enums::AnalogTestStatus>(core::safeStoi(fields[0], "test_status", "@ANALOG", 0));
        if (fields.size() > 1) measured_value = core::safeStodOptional(fields[1], "measured_value", "@ANALOG");
    } else {
        if (fields.size() > 0) test_status = static_cast<enums::AnalogTestStatus>(core::safeStoi(fields[0], "test_status", "@ANALOG", 0));
        if (fields.size() > 1) measured_value = core::safeStodOptional(fields[1], "measured_value", "@ANALOG");
        if (fields.size() > 2 && !fields[2].empty()) {
            subtest_designator = fields[2];
            subtest_designator->erase(remove(subtest_designator->begin(), subtest_designator->end(), '\r'), subtest_designator->end());
            subtest_designator->erase(remove(subtest_designator->begin(), subtest_designator->end(), '\n'), subtest_designator->end());
        }
    }
    // limit_record is handled by the parser in a nested manner, not parsed here
}
json AnalogTestRecord::toJson() const {
    json j = LogRecord::toJson(); // get base class prefix and raw
    j[JSON_KEY_TEST_STATUS] = static_cast<int>(test_status);
    if (measured_value.has_value()) {
        j[JSON_KEY_MEASURED_VALUE] = measured_value.value();
    }
    // Only include subtest_designator if it exists and is not empty
    if (subtest_designator.has_value() && !subtest_designator.value().empty()) {
        j[JSON_KEY_SUBTEST_DESIGNATOR] = subtest_designator.value();
    }
    return j;
}

// Other subclasses only need to call AnalogTestRecord::fromFields
#define IMPL_ANALOG_SUBCLASS_FROMFIELDS(CLASS) \
void CLASS::fromFields(const vector<string>& fields) { AnalogTestRecord::fromFields(fields); }

IMPL_ANALOG_SUBCLASS_FROMFIELDS(CapacitorRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(DiodeRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(FuseRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(InductorRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(JumperRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(MeasureRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(NFetRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(NpnTransistorRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(PFetRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(PnpTransistorRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(PotentiometerRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(ResistorRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(SwitchRecord)
IMPL_ANALOG_SUBCLASS_FROMFIELDS(ZenerDiodeRecord)

#undef IMPL_ANALOG_SUBCLASS_FROMFIELDS

} // namespace records
} // namespace i3070
