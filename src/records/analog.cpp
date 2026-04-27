#include "ate_parser/records/analog.hpp"
#include "ate_parser/utils/errors.hpp"
#include <stdexcept>

namespace ate::records {

bool Lim3Record::has_symmetric_tolerance() const noexcept {
    return positive_tolerance() == negative_tolerance();
}

double Lim3Record::percent_deviation(double measured) const {
    if (nominal_value == 0.0) {
        throw std::domain_error("Lim3Record::percent_deviation: nominal_value is zero");
    }
    return (measured - nominal_value) / nominal_value * 100.0;
}

bool is_analog_test_prefix(enums::Prefix p) noexcept {
    return enums::is_analog_test(p);
}

bool expects_lim3(enums::Prefix p) {
    using enums::Prefix;
    switch (p) {
        case Prefix::a_cap: case Prefix::a_dio: case Prefix::a_ind: case Prefix::a_mea:
        case Prefix::a_pot: case Prefix::a_res: case Prefix::a_zen:
            return true;
        case Prefix::a_fus: case Prefix::a_jum: case Prefix::a_swi:
        case Prefix::a_nfe: case Prefix::a_npn: case Prefix::a_pfe: case Prefix::a_pnp:
            return false;
        default:
            throw std::invalid_argument("expects_lim3: not an analog test prefix");
    }
}

} // namespace ate::records
