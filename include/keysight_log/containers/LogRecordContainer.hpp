/**
 * @file LogRecordContainer.hpp
 * @brief Manages hierarchical grouping of log records and subrecords.
 * 
 * This container class represents one log record node, including its
 * main record and any subordinate subrecords, reflecting the nested
 * hierarchy described in the i3070 Log Record Format (Chapter 8).
 * 
 * Based on: Keysight i3070 Log Record Format, “Hierarchy of Log Records” [1].
 * @date 2025-06-19
 */

#ifndef KEYSIGHT_LOG_CONTAINERS_LOGRECORDCONTAINER_HPP
#define KEYSIGHT_LOG_CONTAINERS_LOGRECORDCONTAINER_HPP


#include "../core/LogRecord.hpp"
#include <memory>
#include <vector>
using namespace std;

namespace keysight_log {
namespace containers {

/**
 * @class LogRecordContainer
 * @brief Node in a tree representing a log record and its subrecords.
 *
 * Each container holds one log record and zero or more child containers
 * for nested subrecords. This mirrors the brace-based hierarchy of
 * i3070 log files, where subrecords appear between the opening brace
 * of a record and its matching closing brace [1].
 */
class LogRecordContainer {
public:
    /**
     * @brief Construct an empty container.
     */
    LogRecordContainer();

    /**
     * @brief Construct a container for a given log record.
     * @param record Pointer to the parsed log record object.
     */
    explicit LogRecordContainer(unique_ptr<core::LogRecord> record);

    /**
     * @brief Add a subrecord container as a child of this node.
     * @param subrecord Unique pointer to the child container.
     */
    void addSubrecord(unique_ptr<LogRecordContainer> subrecord);

    /**
     * @brief Retrieve the main log record.
     * @return Pointer to the contained LogRecord, or nullptr if none.
     */
    core::LogRecord* getRecord() const;

    /**
     * @brief Access all child subrecord containers.
     * @return Const reference to vector of child containers.
     */
    const vector<unique_ptr<LogRecordContainer>>& getSubrecords() const;

    /**
     * @brief Check if this container has any subrecords.
     * @return true if at least one subrecord exists, false otherwise.
     */
    bool hasSubrecords() const;

    /**
     * @brief Clear all subrecords from this container.
     */
    void clearSubrecords();

private:
    unique_ptr<core::LogRecord> record_;                     ///< Owned main log record  
    vector<unique_ptr<LogRecordContainer>> subrecords_; ///< Owned subrecord containers  
};

} // namespace containers
} // namespace keysight_log

#endif // KEYSIGHT_LOG_CONTAINERS_LOGRECORDCONTAINER_HPP
