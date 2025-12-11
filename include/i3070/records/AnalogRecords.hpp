/**
 * @file AnalogRecords.hpp
 * @brief Defines analog test record structures for Keysight i3070 ICT system
 * 
 * This header contains the complete set of analog test record structures used by
 * the Keysight i3070 ICT system. All analog test records share a common format
 * with test status, measured value, and optional subtest designator fields.
 * Each analog test type can be accompanied by limit records (LIM2 or LIM3).
 * 
 * @author Keysight Log Parser Team
 * @version 1.0
 * @date 2025-06-19
 */

#ifndef I3070_RECORDS_ANALOGRECORDS_HPP
#define I3070_RECORDS_ANALOGRECORDS_HPP

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include "../enums/LogRecordPrefix.hpp"
#include "../enums/TestStatus.hpp"
#include "../core/LogRecord.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace records {

// Forward declarations
class LogRecord;
class LogRecordContainer;

/**
 * @class LimitRecord
 * @brief Base class for limit records (LIM2 and LIM3)
 * 
 * Abstract base class for limit records that provide test limit information
 * for analog component tests.
 */
class LimitRecord : public core::LogRecord {
public:
    /**
     * @brief Constructor with prefix
     */
    explicit LimitRecord(i3070::enums::LogRecordPrefix prefix_type)
        : LogRecord(prefix_type) {}
    /**
     * @brief Virtual destructor
     */
    virtual ~LimitRecord() = default;
    
    /**
     * @brief Gets the limit record type
     * @return String description of the limit record type
     */
    virtual string getLimitType() const = 0;
    
    /**
     * @brief Checks if this limit record has nominal value
     * @return true if nominal value is available, false otherwise
     */
    virtual bool hasNominalValue() const = 0;
    
    /**
     * @brief Gets the high limit value
     * @return The high limit value
     */
    virtual double getHighLimit() const = 0;
    
    /**
     * @brief Gets the low limit value
     * @return The low limit value
     */
    virtual double getLowLimit() const = 0;

    virtual bool isWithinLimits(double measured_value) const = 0;

    void fromFields(const vector<string>& fields) override;
    json toJson() const override;
};

/**
 * @class Lim2Record
 * @brief High/low limits record (LIM2)
 * 
 * Contains high and low limits for analog tests. Used by diode, fuse, jumper,
 * measure, nfetr, npn, pfetr, pnp, and switch statements.
 */
class Lim2Record : public LimitRecord {
public:
    // ========================================================================
    // LIM2 FIELDS
    // ========================================================================
    
    double high_limit;      ///< Upper limit for allowable range
    double low_limit;       ///< Lower limit for allowable range
    
    /**
     * @brief Constructor
     */
    Lim2Record();
    
    /**
     * @brief Constructor with limit values
     * @param high_limit_val Upper limit value
     * @param low_limit_val Lower limit value
     */
    Lim2Record(double high_limit_val, double low_limit_val);
    
    /**
     * @brief Gets the limit record type
     * @return "LIM2"
     */
    string getLimitType() const override;
    
    /**
     * @brief Checks if nominal value is available
     * @return false (LIM2 records do not contain nominal values)
     */
    bool hasNominalValue() const override;
    
    /**
     * @brief Gets the high limit value
     * @return high_limit
     */
    double getHighLimit() const override;
    
    /**
     * @brief Gets the low limit value
     * @return low_limit
     */
    double getLowLimit() const override;
    
    /**
     * @brief Checks if a measured value is within limits
     * @param measured_value The value to check
     * @return true if value is within limits, false otherwise
     */
    bool isWithinLimits(double measured_value) const override;
    
    /**
     * @brief Gets the range (high_limit - low_limit)
     * @return The difference between high and low limits
     */
    double getRange() const;
    
    /**
     * @brief Gets the center point of the limits
     * @return The midpoint between high and low limits
     */
    double getCenterPoint() const;

    void fromFields(const vector<string>& fields) override;
    json toJson() const override;
};

/**
 * @class Lim3Record
 * @brief Nominal value and high/low limits record (LIM3)
 * 
 * Contains nominal value and calculated high/low limits for analog tests.
 * Used by capacitor, inductor, potentiometer, resistor, and zener statements.
 */
class Lim3Record : public LimitRecord {
public:
    // ========================================================================
    // LIM3 FIELDS
    // ========================================================================
    
    double nominal_value;   ///< The nominal (expected) value
    double high_limit;      ///< Upper limit (nominal + positive tolerance)
    double low_limit;       ///< Lower limit (nominal + negative tolerance)
    
    /**
     * @brief Constructor
     */
    Lim3Record();
    
    /**
     * @brief Constructor with all values
     * @param nominal_val Nominal value
     * @param high_limit_val Upper limit value
     * @param low_limit_val Lower limit value
     */
    Lim3Record(double nominal_val, double high_limit_val, double low_limit_val);
    
    /**
     * @brief Gets the limit record type
     * @return "LIM3"
     */
    string getLimitType() const override;
    
    /**
     * @brief Checks if nominal value is available
     * @return true (LIM3 records always contain nominal values)
     */
    bool hasNominalValue() const override;
    
    /**
     * @brief Gets the nominal value
     * @return nominal_value
     */
    double getNominalValue() const;
    
    /**
     * @brief Gets the high limit value
     * @return high_limit
     */
    double getHighLimit() const override;
    
    /**
     * @brief Gets the low limit value
     * @return low_limit
     */
    double getLowLimit() const override;
    
    /**
     * @brief Checks if a measured value is within limits
     * @param measured_value The value to check
     * @return true if value is within limits, false otherwise
     */
    bool isWithinLimits(double measured_value) const override;
    
    /**
     * @brief Gets the positive tolerance (high_limit - nominal_value)
     * @return The positive deviation from nominal
     */
    double getPositiveTolerance() const;
    
    /**
     * @brief Gets the negative tolerance (nominal_value - low_limit)
     * @return The negative deviation from nominal
     */
    double getNegativeTolerance() const;
    
    /**
     * @brief Gets the deviation from nominal as a percentage
     * @param measured_value The measured value
     * @return Percentage deviation from nominal value
     */
    double getPercentageDeviation(double measured_value) const;
    
    /**
     * @brief Checks if tolerances are symmetric
     * @return true if positive and negative tolerances are equal, false otherwise
     */
    bool hasSymmetricTolerance() const;

    void fromFields(const vector<string>& fields) override;
    json toJson() const override;
};

/**
 * @class AnalogTestRecord
 * @brief Base class for all analog component test records
 * 
 * This class provides the common structure and functionality for all analog
 * test records in the i3070 system. All analog tests (A-CAP, A-RES, etc.)
 * inherit from this base class and share the same field format.
 */
class AnalogTestRecord : public core::LogRecord {
public:
    // ========================================================================
    // COMMON ANALOG TEST FIELDS
    // These fields are present in all analog test records
    // ========================================================================
    
    enums::AnalogTestStatus test_status;        ///< Test outcome status
    optional<double> measured_value;       ///< Actual measured value (optional)
    optional<string> subtest_designator; ///< Test designator (optional)
    
    // ========================================================================
    // LIMIT SUBRECORDS
    // These optional subrecords provide test limit information
    // ========================================================================
    
    unique_ptr<LimitRecord> limit_record;  ///< Associated limit record (LIM2 or LIM3)

protected:
    /**
     * @brief Protected constructor for derived classes
     * @param prefix_type The specific analog test prefix type
     */
    explicit AnalogTestRecord(enums::LogRecordPrefix prefix_type);

public:
    /**
     * @brief Virtual destructor
     */
    virtual ~AnalogTestRecord() = default;
    
    /**
     * @brief Checks if the test passed
     * @return true if test status indicates pass, false otherwise
     */
    bool isPassed() const;
    
    /**
     * @brief Checks if the test failed
     * @return true if test status indicates failure, false otherwise
     */
    bool isFailed() const;
    
    /**
     * @brief Checks if the test was aborted
     * @return true if test status indicates abort, false otherwise
     */
    bool isAborted() const;
    
    /**
     * @brief Gets a human-readable status description
     * @return String description of the test status
     */
    string getStatusDescription() const;
    
    /**
     * @brief Checks if measured value is available
     * @return true if measured_value has a value, false otherwise
     */
    bool hasMeasuredValue() const;
    
    /**
     * @brief Gets the measured value
     * @return The measured value, or 0.0 if not available
     */
    double getMeasuredValue() const;
    
    /**
     * @brief Checks if subtest designator is available
     * @return true if subtest_designator has a value, false otherwise
     */
    bool hasSubtestDesignator() const;
    
    /**
     * @brief Gets the subtest designator
     * @return The subtest designator string, or empty string if not available
     */
    string getSubtestDesignator() const;
    
    /**
     * @brief Checks if limit record is present
     * @return true if limit_record is available, false otherwise
     */
    bool hasLimitRecord() const;
    
    /**
     * @brief Gets the limit record
     * @return Pointer to the limit record, or nullptr if not available
     */
    LimitRecord* getLimitRecord() const;
    
    /**
     * @brief Sets the limit record
     * @param limit_rec Unique pointer to the limit record
     */
    void setLimitRecord(unique_ptr<LimitRecord> limit_rec);

    void fromFields(const vector<string>& fields) override;
    json toJson() const override;
};

// ============================================================================
// SPECIFIC ANALOG TEST RECORD CLASSES
// Each analog test type has its own derived class for type safety
// ============================================================================

/**
 * @class CapacitorRecord
 * @brief Record for capacitor test results (A-CAP)
 * 
 * Generated by the capacitor statement. Can be followed by LIM3 subrecord
 * containing nominal value and high/low limits for the test.
 */
class CapacitorRecord : public AnalogTestRecord {
public:
    CapacitorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_CAP) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class DiodeRecord
 * @brief Record for diode test results (A-DIO)
 * 
 * Generated by the diode statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class DiodeRecord : public AnalogTestRecord {
public:
    DiodeRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_DIO) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class FuseRecord
 * @brief Record for fuse test results (A-FUS)
 * 
 * Generated by the fuse statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class FuseRecord : public AnalogTestRecord {
public:
    FuseRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_FUS) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class InductorRecord
 * @brief Record for inductor test results (A-IND)
 * 
 * Generated by the inductor statement. Can be followed by LIM3 subrecord
 * containing nominal value and high/low limits for the test.
 */
class InductorRecord : public AnalogTestRecord {
public:
    InductorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_IND) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class JumperRecord
 * @brief Record for jumper test results (A-JUM)
 * 
 * Generated by the jumper statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class JumperRecord : public AnalogTestRecord {
public:
    JumperRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_JUM) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class MeasureRecord
 * @brief Record for measurement results (A-MEA)
 * 
 * Generated by the measure statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test. Note: measure statement can
 * return status 7 (failed) in addition to the standard analog status codes.
 */
class MeasureRecord : public AnalogTestRecord {
public:
    MeasureRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_MEA) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class NFetRecord
 * @brief Record for N-channel FET test results (A-NFE)
 * 
 * Generated by the nfetr statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class NFetRecord : public AnalogTestRecord {
public:
    NFetRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_NFE) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class NpnTransistorRecord
 * @brief Record for NPN transistor test results (A-NPN)
 * 
 * Generated by the npn statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class NpnTransistorRecord : public AnalogTestRecord {
public:
    NpnTransistorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_NPN) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class PFetRecord
 * @brief Record for P-channel FET test results (A-PFE)
 * 
 * Generated by the pfetr statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class PFetRecord : public AnalogTestRecord {
public:
    PFetRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_PFE) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class PnpTransistorRecord
 * @brief Record for PNP transistor test results (A-PNP)
 * 
 * Generated by the pnp statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class PnpTransistorRecord : public AnalogTestRecord {
public:
    PnpTransistorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_PNP) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class PotentiometerRecord
 * @brief Record for potentiometer test results (A-POT)
 * 
 * Generated by the potentiometer statement. Can be followed by LIM3 subrecord
 * containing nominal value and high/low limits for the test.
 */
class PotentiometerRecord : public AnalogTestRecord {
public:
    PotentiometerRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_POT) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class ResistorRecord
 * @brief Record for resistor test results (A-RES)
 * 
 * Generated by the resistor statement. Can be followed by LIM3 subrecord
 * containing nominal value and high/low limits for the test.
 */
class ResistorRecord : public AnalogTestRecord {
public:
    ResistorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_RES) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class SwitchRecord
 * @brief Record for switch test results (A-SWI)
 * 
 * Generated by the switch statement. Can be followed by LIM2 subrecord
 * containing high/low limits for the test.
 */
class SwitchRecord : public AnalogTestRecord {
public:
    SwitchRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_SWI) {}
    void fromFields(const vector<string>& fields) override;
};

/**
 * @class ZenerDiodeRecord
 * @brief Record for Zener diode test results (A-ZEN)
 * 
 * Generated by the zener statement. Can be followed by LIM3 subrecord
 * containing nominal value and high/low limits for the test.
 */
class ZenerDiodeRecord : public AnalogTestRecord {
public:
    ZenerDiodeRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_ZEN) {}
    void fromFields(const vector<string>& fields) override;
};

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Creates an analog test record of the appropriate type
 * @param prefix The log record prefix
 * @return Unique pointer to the created analog test record, or nullptr for non-analog prefixes
 */
unique_ptr<AnalogTestRecord> createAnalogTestRecord(enums::LogRecordPrefix prefix);

/**
 * @brief Checks if a prefix represents an analog test record
 * @param prefix The prefix to check
 * @return true if the prefix is an analog test prefix, false otherwise
 */
bool isAnalogTestPrefix(enums::LogRecordPrefix prefix);

/**
 * @brief Gets the expected limit record type for an analog test
 * @param prefix The analog test prefix
 * @return true if LIM3 record expected, false if LIM2 record expected
 * @throws invalid_argument if prefix is not an analog test prefix
 */
bool expectsLim3Record(enums::LogRecordPrefix prefix);

/**
 * @brief Parses analog test status from integer value
 * @param status_value Integer status value from log record
 * @return Corresponding AnalogTestStatus enum value
 */
enums::AnalogTestStatus parseAnalogTestStatus(int status_value);

/**
 * @brief Converts analog test status to string description
 * @param status The AnalogTestStatus to describe
 * @return Human-readable description of the status
 */
string analogTestStatusToString(enums::AnalogTestStatus status);

/**
 * @brief Validates measured value against limit record
 * @param measured_value The measured value to validate
 * @param limit_record The limit record to validate against
 * @return true if value is within limits, false otherwise
 */
bool validateMeasuredValue(double measured_value, const LimitRecord* limit_record);

} // namespace records
} // namespace i3070

#endif // I3070_RECORDS_ANALOGRECORDS_HPP
