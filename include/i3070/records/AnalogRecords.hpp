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

#pragma once

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include "../enums/LogRecordPrefix.hpp"
#include "../enums/TestStatus.hpp"
#include "../core/LogRecord.hpp"
#include <nlohmann/json.hpp>

namespace i3070::records {

/**
 * @struct LimitRecord
 * @brief Base class for limit records (LIM2 and LIM3)
 */
struct LimitRecord : public core::LogRecord {
    explicit LimitRecord(i3070::enums::LogRecordPrefix prefix_type)
        : LogRecord(prefix_type) {}
    virtual ~LimitRecord() = default;
    
    virtual std::string getLimitType() const = 0;
    virtual bool hasNominalValue() const = 0;
    virtual double getHighLimit() const = 0;
    virtual double getLowLimit() const = 0;
    virtual bool isWithinLimits(double measured_value) const = 0;

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct Lim2Record
 * @brief High/low limits record (LIM2)
 */
struct Lim2Record : public LimitRecord {
    double high_limit;
    double low_limit;
    
    Lim2Record();
    Lim2Record(double high_limit_val, double low_limit_val);
          
    std::string getLimitType() const override;
    bool hasNominalValue() const override;
    double getHighLimit() const override;
    double getLowLimit() const override;
    bool isWithinLimits(double measured_value) const override;
    
    // Additional helper methods found in .cpp
    double getRange() const;
    double getCenterPoint() const;

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct Lim3Record
 * @brief High/low/nominal limits record (LIM3)
 */
struct Lim3Record : public LimitRecord {
    double nominal_value;
    double high_limit;
    double low_limit;
    
    Lim3Record();
    Lim3Record(double nominal_val, double high_limit_val, double low_limit_val);
          
    std::string getLimitType() const override;
    bool hasNominalValue() const override;
    double getNominalValue() const; // Added based on .cpp
    double getHighLimit() const override;
    double getLowLimit() const override;
    bool isWithinLimits(double measured_value) const override;

    // Additional helper methods found in .cpp
    double getPositiveTolerance() const;
    double getNegativeTolerance() const;
    double getPercentageDeviation(double measured_value) const;
    bool hasSymmetricTolerance() const;

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

/**
 * @struct AnalogTestRecord
 * @brief Standard analog test record (@A-T)
 */
struct AnalogTestRecord : public core::LogRecord {
    enums::AnalogTestStatus test_status;
    std::optional<double> measured_value;
    std::optional<std::string> subtest_designator;
    std::unique_ptr<LimitRecord> limit_record; // Owned unique_ptr to match .cpp setLimitRecord(unique_ptr) signature logic usually, but let's check .cpp
    // In .cpp: setLimitRecord(unique_ptr<LimitRecord> limit_rec) { limit_record = std::move(limit_rec); }
    // So it owns it.

    explicit AnalogTestRecord(enums::LogRecordPrefix prefix_type);
          
    bool isPassed() const;
    bool isFailed() const;
    bool isAborted() const;
    std::string getStatusDescription() const;
    
    bool hasMeasuredValue() const;
    double getMeasuredValue() const;
    
    bool hasSubtestDesignator() const;
    std::string getSubtestDesignator() const;
    
    bool hasLimitRecord() const;
    LimitRecord* getLimitRecord() const;
    void setLimitRecord(std::unique_ptr<LimitRecord> limit_rec);

    void fromFields(const std::vector<std::string>& fields) override;
    nlohmann::json toJson() const override;
};

// Subclasses
struct CapacitorRecord : public AnalogTestRecord {
    CapacitorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_CAP) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct DiodeRecord : public AnalogTestRecord {
    DiodeRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_DIO) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct FuseRecord : public AnalogTestRecord {
    FuseRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_FUS) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct InductorRecord : public AnalogTestRecord {
    InductorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_IND) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct JumperRecord : public AnalogTestRecord {
    JumperRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_JUM) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct MeasureRecord : public AnalogTestRecord {
    MeasureRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_MEA) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct NFetRecord : public AnalogTestRecord {
    NFetRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_NFE) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct NpnTransistorRecord : public AnalogTestRecord {
    NpnTransistorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_NPN) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct PFetRecord : public AnalogTestRecord {
    PFetRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_PFE) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct PnpTransistorRecord : public AnalogTestRecord {
    PnpTransistorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_PNP) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct PotentiometerRecord : public AnalogTestRecord {
    PotentiometerRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_POT) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct ResistorRecord : public AnalogTestRecord {
    ResistorRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_RES) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct SwitchRecord : public AnalogTestRecord {
    SwitchRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_SWI) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

struct ZenerDiodeRecord : public AnalogTestRecord {
    ZenerDiodeRecord() : AnalogTestRecord(enums::LogRecordPrefix::A_ZEN) {}
    void fromFields(const std::vector<std::string>& fields) override;
};

} // namespace i3070::records
