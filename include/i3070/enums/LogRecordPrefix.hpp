/**
 * @file LogRecordPrefix.hpp
 * @brief Defines all log record prefixes used in Keysight i3070 ICT system
 * 
 * This header contains the enumeration of all possible log record prefixes
 * that can appear in Keysight i3070 ICT log files. Each prefix identifies
 * a specific type of test result or system information record.
 * 
 * @author 
 * @version 1.0
 * @date 2025-06-19
 */

#ifndef I3070_ENUMS_LOGRECORDPREFIX_HPP
#define I3070_ENUMS_LOGRECORDPREFIX_HPP

#include <string>
#include <unordered_map>

using namespace std;

namespace i3070 {
namespace enums {

/**
 * @enum LogRecordPrefix
 * @brief Enumeration of all possible log record prefixes in i3070 ICT system
 * 
 * This enumeration defines all the prefix types that can appear in log records.
 * Each prefix corresponds to a specific test type or system operation result.
 * The prefixes are organized by category for better maintainability.
 */
enum class LogRecordPrefix {
    // ========================================================================
    // ANALOG TEST RECORDS
    // These prefixes identify various analog component test results
    // ========================================================================
    
    A_CAP,      ///< Capacitor test result record
    A_DIO,      ///< Diode test result record  
    A_FUS,      ///< Fuse test result record
    A_IND,      ///< Inductor test result record
    A_JUM,      ///< Jumper test result record
    A_MEA,      ///< Measurement result record
    A_NFE,      ///< N-channel FET test result record
    A_NPN,      ///< NPN transistor test result record
    A_PFE,      ///< P-channel FET test result record
    A_PNP,      ///< PNP transistor test result record
    A_POT,      ///< Potentiometer test result record
    A_RES,      ///< Resistor test result record
    A_SWI,      ///< Switch test result record
    A_ZEN,      ///< Zener diode test result record
    
    // ========================================================================
    // SYSTEM AND BATCH RECORDS
    // These prefixes identify system-level information and batch processing data
    // ========================================================================
    
    ALM,        ///< Real-time alarm identification record
    AID,        ///< Alarm-inducing board identification record
    ARRAY,      ///< Digitizer result analysis record
    BATCH,      ///< Batch identification and configuration record
    BLOCK,      ///< Test block identification record
    
    // ========================================================================
    // BOUNDARY SCAN RECORDS
    // These prefixes are related to boundary scan test operations
    // ========================================================================
    
    BS_CON,     ///< Boundary scan test description record
    BS_O,       ///< Boundary scan open pins list record
    BS_S,       ///< Boundary scan short pins list record
    BTEST,      ///< Board test description and results record
    
    // ========================================================================
    // DIGITAL TEST RECORDS
    // These prefixes identify digital test results and programming operations
    // ========================================================================
    
    CCHK,       ///< ConnectCheck test result record
    DPIN,       ///< Device pin list for single device record
    D_PLD,      ///< PLD (Programmable Logic Device) programming result record
    D_T,        ///< Digital test result record
    
    // ========================================================================
    // SHORTS AND OPENS TEST RECORDS
    // These prefixes are related to connectivity testing (shorts/opens)
    // ========================================================================
    
    TS,         ///< Shorts test result record (main)
    TS_S,       ///< Shorts test result record (source)
    TS_D,       ///< Shorts test result record (destination)
    TS_O,       ///< Opens test result record
    TS_P,       ///< Phantom shorts list record
    
    // ========================================================================
    // DIAGNOSTIC AND ANALYSIS RECORDS
    // These prefixes provide diagnostic information and failure analysis data
    // ========================================================================
    
    INDICT,     ///< List of potentially faulty devices record
    NETV,       ///< Network verification record
    NODE,       ///< Node list record
    PCHK,       ///< Polarity check test result record
    PIN,        ///< Pin list record
    PF,         ///< Pin fault result record
    PRB,        ///< Probe fault result record
    RETEST,     ///< Retest clear indication record
    RPT,        ///< Report record message
    TJET,       ///< VTEP or TestJet result record
    
    // ========================================================================
    // LIMIT AND SPECIFICATION RECORDS
    // These prefixes define test limits and specifications
    // ========================================================================
    
    LIM2,       ///< Analog test high/low limits record
    LIM3,       ///< Analog test nominal and tolerance limits record
    
    // ========================================================================
    // UNKNOWN OR CUSTOM RECORDS
    // This is used for unrecognized or custom prefix types
    // ========================================================================
    
    UNKNOWN     ///< Unknown or unrecognized prefix type
};

/**
 * @brief Converts a string prefix to LogRecordPrefix enum value
 * @param prefix_str The string representation of the prefix (e.g., "@A-CAP")
 * @return The corresponding LogRecordPrefix enum value
 * @note Returns LogRecordPrefix::UNKNOWN for unrecognized prefixes
 */
LogRecordPrefix stringToPrefix(const string& prefix_str);

/**
 * @brief Converts a LogRecordPrefix enum value to its string representation
 * @param prefix The LogRecordPrefix enum value
 * @return The string representation of the prefix (e.g., "@A-CAP")
 */
string prefixToString(LogRecordPrefix prefix);

/**
 * @brief Checks if a prefix represents an analog test record
 * @param prefix The LogRecordPrefix to check
 * @return true if the prefix is an analog test record, false otherwise
 */
bool isAnalogTestPrefix(LogRecordPrefix prefix);

/**
 * @brief Checks if a prefix represents a digital test record
 * @param prefix The LogRecordPrefix to check
 * @return true if the prefix is a digital test record, false otherwise
 */
bool isDigitalTestPrefix(LogRecordPrefix prefix);

/**
 * @brief Checks if a prefix represents a shorts/opens test record
 * @param prefix The LogRecordPrefix to check
 * @return true if the prefix is a shorts/opens test record, false otherwise
 */
bool isShortsTestPrefix(LogRecordPrefix prefix);

/**
 * @brief Checks if a prefix represents a system or batch record
 * @param prefix The LogRecordPrefix to check
 * @return true if the prefix is a system or batch record, false otherwise
 */
bool isSystemRecordPrefix(LogRecordPrefix prefix);

/**
 * @brief Gets a human-readable description of the prefix
 * @param prefix The LogRecordPrefix to describe
 * @return A descriptive string explaining what the prefix represents
 */
string getPrefixDescription(LogRecordPrefix prefix);

/**
 * @brief Gets the category name for a given prefix
 * @param prefix The LogRecordPrefix to categorize
 * @return The category name (e.g., "Analog Test", "Digital Test", etc.)
 */
string getPrefixCategory(LogRecordPrefix prefix);

} // namespace enums
} // namespace i3070

#endif // I3070_ENUMS_LOGRECORDPREFIX_HPP
