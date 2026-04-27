/**
 * @file prefix.hpp
 * @brief Log record prefix enum and string conversions.
 *
 * Each prefix corresponds to a specific record type produced by Keysight
 * i3070 ICT. The integer values are stable -- they are used as indices in
 * `Parser` switch statements -- but should not be relied on by external
 * code; always use the named enumerators.
 */
#pragma once

#include <string>
#include <string_view>

namespace ate::enums {

enum class Prefix {
    // Analog
    a_cap, a_dio, a_fus, a_ind, a_jum, a_mea, a_nfe, a_npn,
    a_pfe, a_pnp, a_pot, a_res, a_swi, a_zen,
    // System / batch
    alm, aid, array, batch, block,
    // Boundary scan
    bs_con, bs_o, bs_s, btest,
    // Digital
    cchk, dpin, d_pld, d_t,
    // Shorts / opens
    ts, ts_s, ts_d, ts_o, ts_p,
    // Diagnostic / analysis
    indict, lim2, lim3, netv, node, pchk, pf, pin, prb, retest, rpt, tjet,
    // Sentinel
    unknown,
};

[[nodiscard]] Prefix      to_prefix(std::string_view s) noexcept;
[[nodiscard]] std::string to_string(Prefix p);

[[nodiscard]] bool is_analog_test(Prefix p)  noexcept;
[[nodiscard]] bool is_digital_test(Prefix p) noexcept;
[[nodiscard]] bool is_shorts_test(Prefix p)  noexcept;
[[nodiscard]] bool is_system_record(Prefix p) noexcept;

} // namespace ate::enums
