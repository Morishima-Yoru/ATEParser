#pragma once

#include <string>
#include <vector>
#include "../enums/LogRecordPrefix.hpp"
#include <nlohmann/json.hpp>

namespace i3070::core {

/**
 * @struct LogRecord
 * @brief Abstract base structure for all log records, providing prefix and raw data fields.
 */
struct LogRecord {
    LogRecord(enums::LogRecordPrefix prefix = enums::LogRecordPrefix::UNKNOWN, 
              const std::string& raw_prefix = "")
        : prefix(prefix), raw_prefix(raw_prefix), raw_data("") {}
    virtual ~LogRecord() = default;

    // Log record type prefix
    enums::LogRecordPrefix prefix;
    // Raw prefix string (e.g., @PF, @BTEST)
    std::string raw_prefix;
    // Raw log string (optional, for debug or restoration)
    std::string raw_data;

    static bool show_raw_field;
    static bool show_unimplemented_prefix;

    // Get prefix string
    static std::string prefixToString(enums::LogRecordPrefix prefix);

    /**
     * @brief Parses fields and populates members (subclasses must override).
     * @param fields Vector of field strings split from log.
     */
    virtual void fromFields(const std::vector<std::string>& fields);

    /**
     * @brief Converts record to JSON format (subclasses must override).
     * @return JSON object.
     */
    virtual nlohmann::json toJson() const;
};

} // namespace i3070::core
