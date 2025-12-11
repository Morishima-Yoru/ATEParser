/**
 * @file TestStatus.cpp
 * @brief Implements conversion and utility functions for test status enums
 *
 * This source file defines:
 *  - String↔enum conversions for each status type
 *  - Parsing integer codes into enum values
 *  - Severity checks and descriptive helpers
 *
 * Based strictly on Keysight i3070 Log Record Format (Tables 8-4 to 8-51)[1].
 */

#include "i3070/enums/TestStatus.hpp"
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <vector>

using namespace std;
using json = nlohmann::json;

namespace i3070 {
namespace enums {

// ---------------- AnalogTestStatus ----------------

string analogTestStatusToString(AnalogTestStatus status) {
    switch (status) {
        case AnalogTestStatus::PASSED:                  return "Passed";
        case AnalogTestStatus::FAILED:                  return "Failed";
        case AnalogTestStatus::FAILED_COMPLIANCE_LIMIT: return "Failed (Compliance Limit)";
        case AnalogTestStatus::FAILED_DETECTOR_TIMEOUT: return "Failed (Detector Timeout)";
        case AnalogTestStatus::FAILED_GENERAL:          return "Failed (General)";
        case AnalogTestStatus::ABORTED_BY_OPERATOR:     return "Aborted by Operator";
        default:                                        return "Unknown Analog Status";
    }
}

AnalogTestStatus intToAnalogTestStatus(int value) {
    switch (value) {
        case 0:  return AnalogTestStatus::PASSED;
        case 1:  return AnalogTestStatus::FAILED;
        case 2:  return AnalogTestStatus::FAILED_COMPLIANCE_LIMIT;
        case 3:  return AnalogTestStatus::FAILED_DETECTOR_TIMEOUT;
        case 7:  return AnalogTestStatus::FAILED_GENERAL;
        case 11: return AnalogTestStatus::ABORTED_BY_OPERATOR;
        default: throw std::out_of_range("Invalid AnalogTestStatus code");
    }
}

int getStatusSeverity(AnalogTestStatus status) {
    switch (status) {
        case AnalogTestStatus::PASSED:                  return 0;
        case AnalogTestStatus::FAILED:                  return 1;
        case AnalogTestStatus::FAILED_COMPLIANCE_LIMIT: return 2;
        case AnalogTestStatus::FAILED_DETECTOR_TIMEOUT: return 2;
        case AnalogTestStatus::FAILED_GENERAL:          return 2;
        case AnalogTestStatus::ABORTED_BY_OPERATOR:     return 3;
        default:                                        return 3;
    }
}

// ---------------- BoardTestStatus ----------------

string boardTestStatusToString(BoardTestStatus status) {
    switch (status) {
        case BoardTestStatus::PASSED:                   return "Passed";
        case BoardTestStatus::UNCATEGORIZED_FAILURE:    return "Uncategorized Failure";
        case BoardTestStatus::FAILED_PIN_TEST:          return "Failed Pin Test";
        case BoardTestStatus::FAILED_LEARN_MODE:        return "Failed Learn Mode";
        case BoardTestStatus::FAILED_SHORTS_TEST:       return "Failed Shorts Test";
        case BoardTestStatus::FAILED_ANALOG_TEST:       return "Failed Analog Test";
        case BoardTestStatus::FAILED_POWER_SUPPLY:      return "Failed Power Supply";
        case BoardTestStatus::FAILED_DIGITAL_BOUNDARY:  return "Failed Digital/Boundary";
        case BoardTestStatus::FAILED_FUNCTIONAL:        return "Failed Functional";
        case BoardTestStatus::FAILED_PRE_SHORTS:        return "Failed Pre-Shorts";
        case BoardTestStatus::FAILED_BOARD_HANDLER:     return "Failed Board Handler";
        case BoardTestStatus::FAILED_BARCODE:           return "Failed Barcode";
        case BoardTestStatus::XOUT_NOT_TESTED:          return "X'd Out Not Tested";
        case BoardTestStatus::FAILED_VTEP_TESTJET:      return "Failed VTEP/TestJet";
        case BoardTestStatus::FAILED_POLARITY_CHECK:    return "Failed Polarity Check";
        case BoardTestStatus::FAILED_CONNECT_CHECK:     return "Failed ConnectCheck";
        case BoardTestStatus::FAILED_ANALOG_CLUSTER:    return "Failed Analog Cluster";
        case BoardTestStatus::RUNTIME_ERROR:            return "Runtime Error";
        case BoardTestStatus::ABORTED_STOP:             return "Aborted Stop";
        case BoardTestStatus::ABORTED_BREAK:            return "Aborted Break";
        default:                                        return "Unknown Board Status";
    }
}

BoardTestStatus intToBoardTestStatus(int value) {
    if (value >= 0 && value <= 99) {
        return static_cast<BoardTestStatus>(value);
    }
    throw std::out_of_range("Invalid BoardTestStatus code");
}

bool isBoardTestPassed(BoardTestStatus status) {
    return status == BoardTestStatus::PASSED;
}

bool isBoardTestFailed(BoardTestStatus status) {
    return status >= BoardTestStatus::UNCATEGORIZED_FAILURE && status <= BoardTestStatus::FAILED_PRE_SHORTS;
}

bool isBoardTestBogus(BoardTestStatus status) {
    return status >= BoardTestStatus::FAILED_BOARD_HANDLER && status <= BoardTestStatus::ABORTED_BREAK;
}

bool isBoardTestUserDefinable(BoardTestStatus status) {
    return status >= BoardTestStatus::USER_DEFINABLE_MIN && status <= BoardTestStatus::USER_DEFINABLE_MAX;
}

int getStatusSeverity(BoardTestStatus status) {
    if (isBoardTestPassed(status)) return 0;
    if (isBoardTestFailed(status)) return 1;
    if (isBoardTestBogus(status))  return 2;
    return 3;
}

// ---------------- DigitalTestStatus ----------------

string digitalTestStatusToString(DigitalTestStatus status) {
    switch (status) {
        case DigitalTestStatus::PASSED:                return "Passed";
        case DigitalTestStatus::FAILED:                return "Failed";
        case DigitalTestStatus::CRC_RELATED_FAILURE:   return "CRC Failure";
        case DigitalTestStatus::FATAL_ERROR:           return "Fatal Error";
        case DigitalTestStatus::CHAIN_INTEGRITY_FAILURE:return "Chain Integrity Failure";
        default:                                       return "Unknown Digital Status";
    }
}

DigitalTestStatus intToDigitalTestStatus(int value) {
    switch (value) {
        case 0: return DigitalTestStatus::PASSED;
        case 1: return DigitalTestStatus::FAILED;
        case 5: return DigitalTestStatus::CRC_RELATED_FAILURE;
        case 7: return DigitalTestStatus::FATAL_ERROR;
        case 8: return DigitalTestStatus::CHAIN_INTEGRITY_FAILURE;
        default: throw std::out_of_range("Invalid DigitalTestStatus code");
    }
}

int getStatusSeverity(DigitalTestStatus status) {
    switch (status) {
        case DigitalTestStatus::PASSED: return 0;
        case DigitalTestStatus::FAILED: return 1;
        default:                        return 2;
    }
}

// -------------- GenericTestStatus ----------------

string genericTestStatusToString(GenericTestStatus status) {
    switch (status) {
        case GenericTestStatus::PASS:       return "Pass";
        case GenericTestStatus::FAIL:       return "Fail";
        case GenericTestStatus::FATAL_ERROR:return "Fatal Error";
        default:                            return "Unknown Generic Status";
    }
}

GenericTestStatus intToGenericTestStatus(int value) {
    switch (value) {
        case 0: return GenericTestStatus::PASS;
        case 1: return GenericTestStatus::FAIL;
        case 7: return GenericTestStatus::FATAL_ERROR;
        default: throw std::out_of_range("Invalid GenericTestStatus code");
    }
}

int getStatusSeverity(GenericTestStatus status) {
    return (status == GenericTestStatus::PASS) ? 0 : 1;
}

// --------------- DigitalTestSubstatus ----------------

vector<DigitalTestSubstatus> decodeDigitalTestSubstatus(int val) {
    vector<DigitalTestSubstatus> flags;
    for (int bit = 0; bit < 6; ++bit) {
        if (val & (1 << bit)) {
            flags.push_back(static_cast<DigitalTestSubstatus>(1 << bit));
        }
    }
    return flags;
}

int encodeDigitalTestSubstatus(const vector<DigitalTestSubstatus>& flags) {
    int val = 0;
    for (auto f : flags) {
        val |= static_cast<int>(f);
    }
    return val;
}

// ------------- BoundaryScanStatus -----------------

string boundaryScanStatusToString(BoundaryScanStatus status) {
    switch (status) {
        case BoundaryScanStatus::PASS:         return "Pass";
        case BoundaryScanStatus::FAIL:         return "Fail";
        case BoundaryScanStatus::CHAIN_FAILURE: return "Chain Failure";
        default:                               return "Unknown BS Status";
    }
}

// --------------- ShortsTestStatus ----------------

string shortsTestStatusToString(ShortsTestStatus status) {
    switch (status) {
        case ShortsTestStatus::PASSED:         return "Passed";
        case ShortsTestStatus::FAILED:         return "Failed";
        case ShortsTestStatus::LEARNING_PASSED:return "Learning Passed";
        default:                               return "Unknown Shorts Status";
    }
}

// --------------- ArrayTestStatus ----------------

string arrayTestStatusToString(ArrayTestStatus status) {
    switch (status) {
        case ArrayTestStatus::PASS:           return "Pass";
        case ArrayTestStatus::FAIL:           return "Fail";
        case ArrayTestStatus::ERROR_OCCURRED: return "Error Occurred";
        default:                              return "Unknown Array Status";
    }
}

// --------------- PinTestStatus ----------------

string pinTestStatusToString(PinTestStatus status) {
    return (status == PinTestStatus::PASSED) ? "Passed" : "Failed";
}

} // namespace enums
} // namespace i3070
