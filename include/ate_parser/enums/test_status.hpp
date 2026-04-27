/**
 * @file test_status.hpp
 * @brief Test status enumerations + string/integer conversion helpers.
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace ate::enums {

enum class AnalogTestStatus : std::uint8_t {
    passed                    = 0,
    failed                    = 1,
    failed_compliance_limit   = 2,
    failed_detector_timeout   = 3,
    failed_general            = 7,
    aborted_by_operator       = 11,
};

enum class DigitalTestStatus : std::uint8_t {
    passed                    = 0,
    failed                    = 1,
    crc_related_failure       = 5,
    fatal_error               = 7,
    chain_integrity_failure   = 8,
};

enum class GenericTestStatus : std::uint8_t {
    pass             = 0,
    fail             = 1,
    fatal_error      = 7,
    learning_passed  = 20,
};

[[nodiscard]] std::string to_string(AnalogTestStatus s);
[[nodiscard]] std::string to_string(DigitalTestStatus s);
[[nodiscard]] std::string to_string(GenericTestStatus s);

[[nodiscard]] AnalogTestStatus  to_analog_status(int v);   // throws std::out_of_range on unknown
[[nodiscard]] DigitalTestStatus to_digital_status(int v);
[[nodiscard]] GenericTestStatus to_generic_status(int v);

[[nodiscard]] std::vector<int> decode_digital_substatus(int val);
[[nodiscard]] int              encode_digital_substatus(const std::vector<int>& flags);

} // namespace ate::enums
