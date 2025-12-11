/**
 * @file SafeConversion.hpp
 * @brief Safe type conversion utilities for parsing log records
 * 
 * This header provides wrapper functions for stoi, stod, etc.
 * that catch exceptions and provide detailed error messages for debugging.
 * 
 * @author Keysight Log Parser Team
 * @version 1.0
 * @date 2025-06-19
 */

#ifndef I3070_CORE_SAFECONVERSION_HPP
#define I3070_CORE_SAFECONVERSION_HPP

#include <string>
#include <optional>
#include <cstdint>

using namespace std;

namespace i3070 {
namespace core {

/**
 * @brief Safely convert string to integer with detailed error reporting
 * @param str The string to convert
 * @param field_name Name of the field for error reporting
 * @param record_type Type of record for error reporting
 * @param default_value Default value to return if conversion fails
 * @return Converted integer or default value
 */
int safeStoi(const string& str, const string& field_name, 
             const string& record_type, int default_value = 0);

             
/**
 * @brief Safely convert string to unsigned longlong with detailed error reporting
 * @param str The string to convert
 * @param field_name Name of the field for error reporting
 * @param record_type Type of record for error reporting
 * @param default_value Default value to return if conversion fails
 * @return Converted unsigned longlong or default value
 */
uint64_t safeStoull(const string& str, const string& field_name, 
                    const string& record_type, uint64_t default_value = 0);

/**
 * @brief Safely convert string to double with detailed error reporting
 * @param str The string to convert
 * @param field_name Name of the field for error reporting
 * @param record_type Type of record for error reporting
 * @param default_value Default value to return if conversion fails
 * @return Converted double or default value
 */
double safeStod(const string& str, const string& field_name, 
                const string& record_type, double default_value = 0.0);

/**
 * @brief Safely convert string to boolean with detailed error reporting
 * @param str The string to convert
 * @param field_name Name of the field for error reporting
 * @param record_type Type of record for error reporting
 * @param default_value Default value to return if conversion fails
 * @return Converted boolean or default value
 */
bool safeStob(const string& str, const string& field_name, 
              const string& record_type, bool default_value = false);

/**
 * @brief Safely convert string to optional integer with detailed error reporting
 * @param str The string to convert
 * @param field_name Name of the field for error reporting
 * @param record_type Type of record for error reporting
 * @return optional<int> containing the converted value or nullopt if empty/invalid
 */
optional<int> safeStoiOptional(const string& str, const string& field_name, 
                                   const string& record_type);

/**
 * @brief Safely convert string to optional double with detailed error reporting
 * @param str The string to convert
 * @param field_name Name of the field for error reporting
 * @param record_type Type of record for error reporting
 * @return optional<double> containing the converted value or nullopt if empty/invalid
 */
optional<double> safeStodOptional(const string& str, const string& field_name, 
                                      const string& record_type);

} // namespace core
} // namespace i3070

#endif // I3070_CORE_SAFECONVERSION_HPP 
