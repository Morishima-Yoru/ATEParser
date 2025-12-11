/**
 * @file TestStatus.hpp
 * @brief Defines test status enumerations used in Keysight i3070 ICT system
 * 
 * This header contains comprehensive test status enumerations for different
 * types of tests performed by the Keysight i3070 ICT system. Each test type
 * may have its own specific status codes that indicate the outcome of the test.
 * 
 * @author Keysight Log Parser Team
 * @version 1.0
 * @date 2025-06-19
 */

#pragma once

#include <string>
#include <vector>

namespace i3070::enums {

/**
 * @enum AnalogTestStatus
 * @brief Status codes for analog component tests (A-CAP, A-RES, A-DIO, etc.)
 */
enum class AnalogTestStatus {
    PASSED = 0,                         ///< Test passed successfully
    FAILED = 1,                         ///< Test failed (general failure)
    FAILED_COMPLIANCE_LIMIT = 2,        ///< Test failed due to compliance limit exceeded
    FAILED_DETECTOR_TIMEOUT = 3,        ///< Test failed due to detector timeout
    FAILED_GENERAL = 7,                 ///< General test failure (used by measure statement)
    ABORTED_BY_OPERATOR = 11            ///< Test aborted by operator intervention
};

/**
 * @enum BoardTestStatus
 * @brief Overall board test status codes used in BTEST records
 */
enum class BoardTestStatus {
    PASSED = 0,                         ///< Board test passed successfully
    UNCATEGORIZED_FAILURE = 1,          ///< Uncategorized failure
    FAILED_PIN_TEST = 2,                ///< Failed pin test
    FAILED_LEARN_MODE = 3,              ///< Failed in learn mode
    FAILED_SHORTS_TEST = 4,             ///< Failed shorts test
    RESERVED_5 = 5,                     ///< Reserved status code
    FAILED_ANALOG_TEST = 6,             ///< Failed analog test
    FAILED_POWER_SUPPLY = 7,            ///< Failed power supply test
    FAILED_DIGITAL_BOUNDARY = 8,        ///< Failed digital or boundary scan test
    FAILED_FUNCTIONAL = 9,              ///< Failed functional test
    FAILED_PRE_SHORTS = 10,             ///< Failed pre-shorts test
    FAILED_BOARD_HANDLER = 11,          ///< Failed in board handler (bogus)
    FAILED_BARCODE = 12,                ///< Failed barcode (bogus)
    XOUT_NOT_TESTED = 13,               ///< X'd out board on panel - not tested or missing (bogus)
    FAILED_VTEP_TESTJET = 14,           ///< Failed in VTEP or TestJet (bogus)
    FAILED_POLARITY_CHECK = 15,         ///< Failed in polarity check (bogus)
    FAILED_CONNECT_CHECK = 16,          ///< Failed in ConnectCheck (Mux system only) (bogus)
    FAILED_ANALOG_CLUSTER = 17,         ///< Failed in analog cluster test (bogus)
    // 18-79 reserved
    RUNTIME_ERROR = 80,                 ///< Runtime error (bogus)
    ABORTED_STOP = 81,                  ///< Aborted with STOP (bogus)
    ABORTED_BREAK = 82,                 ///< Aborted with BREAK (bogus)
    // 83-89 reserved
    // 90-99 user-definable
    USER_DEFINABLE_MIN = 90,            ///< Minimum user-definable status code
    USER_DEFINABLE_MAX = 99             ///< Maximum user-definable status code
};

/**
 * @enum DigitalTestStatus
 * @brief Status codes for digital tests (D-T records)
 */
enum class DigitalTestStatus {
    PASSED = 0,                         ///< Digital test passed successfully
    FAILED = 1,                         ///< Digital test failed (see substatus for details)
    CRC_RELATED_FAILURE = 5,            ///< CRC related failure
    FATAL_ERROR = 7,                    ///< Fatal error - test did not complete
    CHAIN_INTEGRITY_FAILURE = 8         ///< Pre or post chain integrity failure
};

/**
 * @enum DigitalTestSubstatus
 * @brief Substatus bit flags for digital tests
 */
enum class DigitalTestSubstatus {
    NONE = 0,                           ///< No substatus flags set
    // Add other values if needed based on spec
};

/**
 * @enum TestStatus
 * @brief General test status for common use
 */
enum class TestStatus {
    PASSED = 0,
    FAILED = 1
};

/**
 * @enum GenericTestStatus
 * @brief Generic test status for shorts/opens
 */
enum class GenericTestStatus {
    PASS = 0,
    FAIL = 1,
    FATAL_ERROR = 7,
    LEARNING_PASSED = 20
};

/**
 * @enum BoundaryScanStatus
 */
enum class BoundaryScanStatus {
    PASS = 0,
    FAIL = 1,
    CHAIN_FAILURE = 2 // Inferred value
};

/**
 * @enum ShortsTestStatus
 */
enum class ShortsTestStatus {
    PASSED = 0,
    FAILED = 1,
    LEARNING_PASSED = 20
};

/**
 * @enum ArrayTestStatus
 */
enum class ArrayTestStatus {
    PASS = 0,
    FAIL = 1,
    ERROR_OCCURRED = 2 // Inferred value
};

/**
 * @enum PinTestStatus
 */
enum class PinTestStatus {
    PASSED = 0,
    FAILED = 1
};

// Function declarations
std::string analogTestStatusToString(AnalogTestStatus status);
AnalogTestStatus intToAnalogTestStatus(int value);
int getStatusSeverity(AnalogTestStatus status);

std::string boardTestStatusToString(BoardTestStatus status);
BoardTestStatus intToBoardTestStatus(int value);
bool isBoardTestPassed(BoardTestStatus status);
bool isBoardTestFailed(BoardTestStatus status);
bool isBoardTestBogus(BoardTestStatus status);
bool isBoardTestUserDefinable(BoardTestStatus status);
int getStatusSeverity(BoardTestStatus status);

std::string digitalTestStatusToString(DigitalTestStatus status);
DigitalTestStatus intToDigitalTestStatus(int value);
int getStatusSeverity(DigitalTestStatus status);

std::string genericTestStatusToString(GenericTestStatus status);
GenericTestStatus intToGenericTestStatus(int value);
int getStatusSeverity(GenericTestStatus status);

std::vector<DigitalTestSubstatus> decodeDigitalTestSubstatus(int val);
int encodeDigitalTestSubstatus(const std::vector<DigitalTestSubstatus>& flags);

std::string boundaryScanStatusToString(BoundaryScanStatus status);
std::string shortsTestStatusToString(ShortsTestStatus status);
std::string arrayTestStatusToString(ArrayTestStatus status);
std::string pinTestStatusToString(PinTestStatus status);

} // namespace i3070::enums
