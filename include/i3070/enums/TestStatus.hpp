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

#ifndef I3070_ENUMS_TESTSTATUS_HPP
#define I3070_ENUMS_TESTSTATUS_HPP

#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

namespace i3070 {
namespace enums {

/**
 * @enum AnalogTestStatus
 * @brief Status codes for analog component tests (A-CAP, A-RES, A-DIO, etc.)
 * 
 * These status codes are used by all analog test records including capacitor,
 * resistor, diode, inductor, and other analog component tests.
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
 * 
 * These status codes represent the overall outcome of testing an entire board.
 * Values 1-10 indicate failing boards, while 11-99 are considered "bogus"
 * (neither passing nor failing) by statistical analysis tools.
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
 * 
 * These status codes are specific to digital component and circuit tests.
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
 * 
 * This represents a 6-bit binary value where each bit indicates a specific
 * failure condition. Multiple conditions can occur simultaneously.
 */
enum class DigitalTestSubstatus {
    NONE = 0,                           ///< No substatus flags set
    FAIL = 1,                           ///< Bit 0: Fail flag
    SAFEGUARD_TIMEOUT = 2,              ///< Bit 1: SAFEGUARD timeout
    HARDWARE_ERROR = 4,                 ///< Bit 2: Hardware error
    PAUSE = 8,                          ///< Bit 3: Pause flag
    HALT = 16,                          ///< Bit 4: Halt flag
    OVERVOLTAGE = 32                    ///< Bit 5: Overvoltage flag
};

/**
 * @enum GenericTestStatus
 * @brief Generic test status used by various test types
 * 
 * This enumeration is used by several test types including boundary scan,
 * VTEP/TestJet, ConnectCheck, and others that use a simple pass/fail/error model.
 */
enum class GenericTestStatus {
    PASS = 0,                           ///< Test passed successfully
    FAIL = 1,                           ///< Test failed
    FATAL_ERROR = 7                     ///< Fatal error occurred
};

/**
 * @enum BoundaryScanStatus
 * @brief Status codes specific to boundary scan tests
 * 
 * Used by BS-CON records to indicate boundary scan test results.
 */
enum class BoundaryScanStatus {
    PASS = 0,                           ///< Boundary scan test passed
    FAIL = 1,                           ///< Boundary scan test failed
    CHAIN_FAILURE = 7                   ///< Boundary scan chain failure
};

/**
 * @enum ShortsTestStatus
 * @brief Status codes for shorts/opens tests
 * 
 * Used by TS records to indicate shorts and opens test results.
 */
enum class ShortsTestStatus {
    PASSED = 0,                         ///< Shorts test passed successfully
    FAILED = 1,                         ///< Shorts test failed
    LEARNING_PASSED = 20                ///< Learning mode passed
};

/**
 * @enum ArrayTestStatus
 * @brief Status codes for digitizer array analysis
 * 
 * Used by ARRAY records for digitizer results analysis.
 */
enum class ArrayTestStatus {
    PASS = 0,                           ///< Array analysis passed
    FAIL = 1,                           ///< Array analysis failed
    ERROR_OCCURRED = 7                  ///< Error occurred during analysis
};

/**
 * @enum PinTestStatus
 * @brief Status codes for pin-related tests
 * 
 * Used by PF (pinsfailed) and PRB (probe) records.
 */
enum class PinTestStatus {
    PASSED = 0,                         ///< Pin test passed
    FAILED = 1                          ///< Pin test failed
};

/**
 * @brief Converts analog test status to string representation
 * @param status The AnalogTestStatus enum value
 * @return String description of the status
 */
string analogTestStatusToString(AnalogTestStatus status);

/**
 * @brief Converts board test status to string representation
 * @param status The BoardTestStatus enum value
 * @return String description of the status
 */
string boardTestStatusToString(BoardTestStatus status);

/**
 * @brief Converts digital test status to string representation
 * @param status The DigitalTestStatus enum value
 * @return String description of the status
 */
string digitalTestStatusToString(DigitalTestStatus status);

/**
 * @brief Converts generic test status to string representation
 * @param status The GenericTestStatus enum value
 * @return String description of the status
 */
string genericTestStatusToString(GenericTestStatus status);

/**
 * @brief Parses integer value to AnalogTestStatus
 * @param value Integer status value from log record
 * @return Corresponding AnalogTestStatus enum value
 * @note Returns AnalogTestStatus::FAILED for unrecognized values
 */
AnalogTestStatus intToAnalogTestStatus(int value);

/**
 * @brief Parses integer value to BoardTestStatus
 * @param value Integer status value from log record
 * @return Corresponding BoardTestStatus enum value
 * @note Returns BoardTestStatus::UNCATEGORIZED_FAILURE for unrecognized values
 */
BoardTestStatus intToBoardTestStatus(int value);

/**
 * @brief Parses integer value to DigitalTestStatus
 * @param value Integer status value from log record
 * @return Corresponding DigitalTestStatus enum value
 * @note Returns DigitalTestStatus::FAILED for unrecognized values
 */
DigitalTestStatus intToDigitalTestStatus(int value);

/**
 * @brief Parses integer value to GenericTestStatus
 * @param value Integer status value from log record
 * @return Corresponding GenericTestStatus enum value
 * @note Returns GenericTestStatus::FAIL for unrecognized values
 */
GenericTestStatus intToGenericTestStatus(int value);

/**
 * @brief Checks if a board test status indicates a passing result
 * @param status The BoardTestStatus to check
 * @return true if the status indicates a passing board, false otherwise
 */
bool isBoardTestPassed(BoardTestStatus status);

/**
 * @brief Checks if a board test status indicates a failing result
 * @param status The BoardTestStatus to check
 * @return true if the status indicates a failing board (codes 1-10), false otherwise
 */
bool isBoardTestFailed(BoardTestStatus status);

/**
 * @brief Checks if a board test status indicates a bogus result
 * @param status The BoardTestStatus to check
 * @return true if the status indicates a bogus result (codes 11-99), false otherwise
 * 
 * Bogus results are neither passing nor failing and typically indicate
 * issues with the test setup, equipment, or process rather than the board itself.
 */
bool isBoardTestBogus(BoardTestStatus status);

/**
 * @brief Checks if a board test status is user-definable
 * @param status The BoardTestStatus to check
 * @return true if the status is in the user-definable range (90-99), false otherwise
 */
bool isBoardTestUserDefinable(BoardTestStatus status);

/**
 * @brief Decodes digital test substatus flags
 * @param substatus_value Integer value containing the substatus flags
 * @return Vector of DigitalTestSubstatus flags that are set
 */
vector<DigitalTestSubstatus> decodeDigitalTestSubstatus(int substatus_value);

/**
 * @brief Encodes digital test substatus flags into integer value
 * @param substatus_flags Vector of DigitalTestSubstatus flags to encode
 * @return Integer value representing the combined flags
 */
int encodeDigitalTestSubstatus(const vector<DigitalTestSubstatus>& substatus_flags);

/**
 * @brief Gets a detailed description of the test status
 * @param status The AnalogTestStatus to describe
 * @return Detailed description string explaining the status
 */
string getAnalogTestStatusDescription(AnalogTestStatus status);

/**
 * @brief Gets a detailed description of the board test status
 * @param status The BoardTestStatus to describe
 * @return Detailed description string explaining the status
 */
string getBoardTestStatusDescription(BoardTestStatus status);

/**
 * @brief Gets a detailed description of the digital test status
 * @param status The DigitalTestStatus to describe
 * @return Detailed description string explaining the status
 */
string getDigitalTestStatusDescription(DigitalTestStatus status);

/**
 * @brief Gets the severity level of a test status
 * @param status The test status to evaluate
 * @return Integer severity level (0=pass, 1=fail, 2=error, 3=abort)
 */
int getStatusSeverity(AnalogTestStatus status);

/**
 * @overload
 */
int getStatusSeverity(BoardTestStatus status);

/**
 * @overload
 */
int getStatusSeverity(DigitalTestStatus status);

/**
 * @overload
 */
int getStatusSeverity(GenericTestStatus status);

} // namespace enums
} // namespace i3070

#endif // I3070_ENUMS_TESTSTATUS_HPP
