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

#pragma once

#include <string>

namespace i3070::enums {

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
    
    INDICT,     ///< Indictment record for identifying failing components
    LIM2,       ///< High/low limit record (2 limits)
    LIM3,       ///< High/low/nominal limit record (3 limits)
    NETV,       ///< Network verification record
    NODE,       ///< Node list record
    PCHK,       ///< Polarity check result record
    PF,         ///< Pins fail record
    PIN,        ///< Pin information record
    PRB,        ///< Probe record? (Found in cpp map)
    RETEST,     ///< Retest information record
    RPT,        ///< Report message record
    TJET,       ///< TestJet test result record
    
    // ========================================================================
    // SPECIAL PREFIXES
    // These are used for fallback or unknown record types
    // ========================================================================
    
    UNKNOWN     ///< Unknown or unrecognized record prefix
};

// Function declarations
LogRecordPrefix stringToPrefix(const std::string& prefix_str);
std::string prefixToString(LogRecordPrefix prefix);
bool isAnalogTestPrefix(LogRecordPrefix prefix);
// Add other declarations if needed by consumers

} // namespace i3070::enums
