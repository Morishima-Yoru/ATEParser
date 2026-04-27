#include "ate_parser/enums/test_status.hpp"
#include <stdexcept>

namespace ate::enums {

std::string to_string(AnalogTestStatus s) {
    switch (s) {
        case AnalogTestStatus::passed:                  return "Passed";
        case AnalogTestStatus::failed:                  return "Failed";
        case AnalogTestStatus::failed_compliance_limit: return "Failed (Compliance Limit)";
        case AnalogTestStatus::failed_detector_timeout: return "Failed (Detector Timeout)";
        case AnalogTestStatus::failed_general:          return "Failed (General)";
        case AnalogTestStatus::aborted_by_operator:     return "Aborted by Operator";
    }
    return "Unknown Analog Status";
}

std::string to_string(DigitalTestStatus s) {
    switch (s) {
        case DigitalTestStatus::passed:                  return "Passed";
        case DigitalTestStatus::failed:                  return "Failed";
        case DigitalTestStatus::crc_related_failure:     return "CRC Failure";
        case DigitalTestStatus::fatal_error:             return "Fatal Error";
        case DigitalTestStatus::chain_integrity_failure: return "Chain Integrity Failure";
    }
    return "Unknown Digital Status";
}

std::string to_string(GenericTestStatus s) {
    switch (s) {
        case GenericTestStatus::pass:            return "Pass";
        case GenericTestStatus::fail:            return "Fail";
        case GenericTestStatus::fatal_error:     return "Fatal Error";
        case GenericTestStatus::learning_passed: return "Learning Passed";
    }
    return "Unknown Generic Status";
}

AnalogTestStatus to_analog_status(int v) {
    switch (v) {
        case 0:  return AnalogTestStatus::passed;
        case 1:  return AnalogTestStatus::failed;
        case 2:  return AnalogTestStatus::failed_compliance_limit;
        case 3:  return AnalogTestStatus::failed_detector_timeout;
        case 7:  return AnalogTestStatus::failed_general;
        case 11: return AnalogTestStatus::aborted_by_operator;
        default: throw std::out_of_range("Invalid AnalogTestStatus code: " + std::to_string(v));
    }
}

DigitalTestStatus to_digital_status(int v) {
    switch (v) {
        case 0: return DigitalTestStatus::passed;
        case 1: return DigitalTestStatus::failed;
        case 5: return DigitalTestStatus::crc_related_failure;
        case 7: return DigitalTestStatus::fatal_error;
        case 8: return DigitalTestStatus::chain_integrity_failure;
        default: throw std::out_of_range("Invalid DigitalTestStatus code: " + std::to_string(v));
    }
}

GenericTestStatus to_generic_status(int v) {
    switch (v) {
        case 0:  return GenericTestStatus::pass;
        case 1:  return GenericTestStatus::fail;
        case 7:  return GenericTestStatus::fatal_error;
        case 20: return GenericTestStatus::learning_passed;
        default: throw std::out_of_range("Invalid GenericTestStatus code: " + std::to_string(v));
    }
}

std::vector<int> decode_digital_substatus(int val) {
    std::vector<int> flags;
    for (int bit = 0; bit < 6; ++bit) {
        const int mask = 1 << bit;
        if (val & mask) flags.push_back(mask);
    }
    return flags;
}

int encode_digital_substatus(const std::vector<int>& flags) {
    int v = 0;
    for (int f : flags) v |= f;
    return v;
}

} // namespace ate::enums
