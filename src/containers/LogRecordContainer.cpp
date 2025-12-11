/**
 * @file LogRecordContainer.cpp
 * @brief Implements the LogRecordContainer class for hierarchical log records.
 *
 * This source file provides definitions for:
 *  - Constructors
 *  - Subrecord management methods
 *  - Accessor functions
 *
 * Based on Keysight i3070 Log Record Format, "Hierarchy of Log Records" (Chapter 8) [1].
 * @date 2025-06-19
 */

#include "i3070/containers/LogRecordContainer.hpp"

namespace i3070 {
namespace containers {

// Default constructor: creates an empty container with no record
LogRecordContainer::LogRecordContainer()
    : record_(nullptr), subrecords_() {}

// Constructor with record: takes ownership of provided LogRecord
LogRecordContainer::LogRecordContainer(std::unique_ptr<core::LogRecord> record)
    : record_(std::move(record)), subrecords_() {}

// Add a child container as a subrecord
void LogRecordContainer::addSubrecord(std::unique_ptr<LogRecordContainer> subrecord) {
    if (subrecord) {
        subrecords_.emplace_back(std::move(subrecord));
    }
}

// Retrieve pointer to the main log record (or nullptr if empty)
core::LogRecord* LogRecordContainer::getRecord() const {
    return record_.get();
}

// Access all child subrecord containers
const std::vector<std::unique_ptr<LogRecordContainer>>&
LogRecordContainer::getSubrecords() const {
    return subrecords_;
}

// Check if any subrecords exist under this container
bool LogRecordContainer::hasSubrecords() const {
    return !subrecords_.empty();
}

// Remove and destroy all child subrecord containers
void LogRecordContainer::clearSubrecords() {
    subrecords_.clear();
}

}  // namespace containers
}  // namespace i3070
