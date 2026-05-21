#include "ate_parser/enums/test_status.hpp"
#include "ate_parser/utils/enum_messages.hpp"
#include <stdexcept>

namespace em = ate::enum_msg;

namespace ate::enums {

std::string to_string(AnalogTestStatus s) {
    switch (s) {
        case AnalogTestStatus::passed:                  return std::string{em::k_analog_passed};
        case AnalogTestStatus::failed:                  return std::string{em::k_analog_failed};
        case AnalogTestStatus::failed_compliance_limit: return std::string{em::k_analog_failed_compliance};
        case AnalogTestStatus::failed_detector_timeout: return std::string{em::k_analog_failed_detector};
        case AnalogTestStatus::failed_general:          return std::string{em::k_analog_failed_general};
        case AnalogTestStatus::aborted_by_operator:     return std::string{em::k_analog_aborted_operator};
    }
    return std::string{em::k_analog_unknown};
}

std::string to_string(DigitalTestStatus s) {
    switch (s) {
        case DigitalTestStatus::passed:                  return std::string{em::k_digital_passed};
        case DigitalTestStatus::failed:                  return std::string{em::k_digital_failed};
        case DigitalTestStatus::crc_related_failure:     return std::string{em::k_digital_crc_failure};
        case DigitalTestStatus::fatal_error:             return std::string{em::k_digital_fatal_error};
        case DigitalTestStatus::chain_integrity_failure: return std::string{em::k_digital_chain_integrity};
    }
    return std::string{em::k_digital_unknown};
}

std::string to_string(GenericTestStatus s) {
    switch (s) {
        case GenericTestStatus::pass:            return std::string{em::k_generic_pass};
        case GenericTestStatus::fail:            return std::string{em::k_generic_fail};
        case GenericTestStatus::fatal_error:     return std::string{em::k_generic_fatal_error};
        case GenericTestStatus::learning_passed: return std::string{em::k_generic_learning_passed};
    }
    return std::string{em::k_generic_unknown};
}

AnalogTestStatus to_analog_status(int v) {
    switch (v) {
        case 0:  return AnalogTestStatus::passed;
        case 1:  return AnalogTestStatus::failed;
        case 2:  return AnalogTestStatus::failed_compliance_limit;
        case 3:  return AnalogTestStatus::failed_detector_timeout;
        case 7:  return AnalogTestStatus::failed_general;
        case 11: return AnalogTestStatus::aborted_by_operator;
        default: throw std::out_of_range(std::string{em::k_invalid_analog_code} + std::to_string(v));
    }
}

DigitalTestStatus to_digital_status(int v) {
    switch (v) {
        case 0: return DigitalTestStatus::passed;
        case 1: return DigitalTestStatus::failed;
        case 5: return DigitalTestStatus::crc_related_failure;
        case 7: return DigitalTestStatus::fatal_error;
        case 8: return DigitalTestStatus::chain_integrity_failure;
        default: throw std::out_of_range(std::string{em::k_invalid_digital_code} + std::to_string(v));
    }
}

GenericTestStatus to_generic_status(int v) {
    switch (v) {
        case 0:  return GenericTestStatus::pass;
        case 1:  return GenericTestStatus::fail;
        case 7:  return GenericTestStatus::fatal_error;
        case 20: return GenericTestStatus::learning_passed;
        default: throw std::out_of_range(std::string{em::k_invalid_generic_code} + std::to_string(v));
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
