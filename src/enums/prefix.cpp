#include "ate_parser/enums/prefix.hpp"
#include <array>
#include <unordered_map>

namespace ate::enums {

namespace {
const std::unordered_map<std::string_view, Prefix>& prefix_table() {
    // Build the reverse-lookup table once from the single source of truth:
    // to_string_view(). Adding a new Prefix only requires updating the enum,
    // to_string_view(), and this array -- the map builds itself automatically.
    static const std::unordered_map<std::string_view, Prefix> t = []() {
        static constexpr std::array all_prefixes = {
            Prefix::a_cap,  Prefix::a_dio,  Prefix::a_fus,  Prefix::a_ind,
            Prefix::a_jum,  Prefix::a_mea,  Prefix::a_nfe,  Prefix::a_npn,
            Prefix::a_pfe,  Prefix::a_pnp,  Prefix::a_pot,  Prefix::a_res,
            Prefix::a_swi,  Prefix::a_zen,
            Prefix::alm,    Prefix::aid,    Prefix::array,  Prefix::batch,
            Prefix::block,  Prefix::bs_con, Prefix::bs_o,   Prefix::bs_s,
            Prefix::btest,  Prefix::cchk,   Prefix::dpin,   Prefix::d_pld,
            Prefix::d_t,    Prefix::ts,     Prefix::ts_s,   Prefix::ts_d,
            Prefix::ts_o,   Prefix::ts_p,   Prefix::indict, Prefix::lim2,
            Prefix::lim3,   Prefix::netv,   Prefix::node,   Prefix::pchk,
            Prefix::pf,     Prefix::pin,    Prefix::prb,    Prefix::retest,
            Prefix::rpt,    Prefix::tjet,
        };
        std::unordered_map<std::string_view, Prefix> m;
        m.reserve(all_prefixes.size());
        for (const auto p : all_prefixes) {
            m.emplace(to_string_view(p), p);
        }
        return m;
    }();
    return t;
}
} // namespace

Prefix to_prefix(std::string_view s) noexcept {
    const auto& t = prefix_table();
    auto it = t.find(s);
    return (it != t.end()) ? it->second : Prefix::unknown;
}

std::string to_string(Prefix p) {
    return std::string{to_string_view(p)};
}

std::string_view to_string_view(Prefix p) noexcept {
    switch (p) {
        case Prefix::a_cap:  return "@A-CAP";
        case Prefix::a_dio:  return "@A-DIO";
        case Prefix::a_fus:  return "@A-FUS";
        case Prefix::a_ind:  return "@A-IND";
        case Prefix::a_jum:  return "@A-JUM";
        case Prefix::a_mea:  return "@A-MEA";
        case Prefix::a_nfe:  return "@A-NFE";
        case Prefix::a_npn:  return "@A-NPN";
        case Prefix::a_pfe:  return "@A-PFE";
        case Prefix::a_pnp:  return "@A-PNP";
        case Prefix::a_pot:  return "@A-POT";
        case Prefix::a_res:  return "@A-RES";
        case Prefix::a_swi:  return "@A-SWI";
        case Prefix::a_zen:  return "@A-ZEN";
        case Prefix::alm:    return "@ALM";
        case Prefix::aid:    return "@AID";
        case Prefix::array:  return "@ARRAY";
        case Prefix::batch:  return "@BATCH";
        case Prefix::block:  return "@BLOCK";
        case Prefix::bs_con: return "@BS-CON";
        case Prefix::bs_o:   return "@BS-O";
        case Prefix::bs_s:   return "@BS-S";
        case Prefix::btest:  return "@BTEST";
        case Prefix::cchk:   return "@CCHK";
        case Prefix::dpin:   return "@DPIN";
        case Prefix::d_pld:  return "@D-PLD";
        case Prefix::d_t:    return "@D-T";
        case Prefix::ts:     return "@TS";
        case Prefix::ts_s:   return "@TS-S";
        case Prefix::ts_d:   return "@TS-D";
        case Prefix::ts_o:   return "@TS-O";
        case Prefix::ts_p:   return "@TS-P";
        case Prefix::indict: return "@INDICT";
        case Prefix::lim2:   return "@LIM2";
        case Prefix::lim3:   return "@LIM3";
        case Prefix::netv:   return "@NETV";
        case Prefix::node:   return "@NODE";
        case Prefix::pchk:   return "@PCHK";
        case Prefix::pf:     return "@PF";
        case Prefix::pin:    return "@PIN";
        case Prefix::prb:    return "@PRB";
        case Prefix::retest: return "@RETEST";
        case Prefix::rpt:    return "@RPT";
        case Prefix::tjet:   return "@TJET";
        case Prefix::unknown:return "@UNKNOWN";
    }
    return "@UNKNOWN";
}

bool is_analog_test(Prefix p) noexcept  { return p >= Prefix::a_cap && p <= Prefix::a_zen; }
bool is_digital_test(Prefix p) noexcept { return p == Prefix::cchk || p == Prefix::dpin || p == Prefix::d_pld || p == Prefix::d_t; }
bool is_shorts_test(Prefix p) noexcept  { return p >= Prefix::ts && p <= Prefix::ts_p; }
bool is_system_record(Prefix p) noexcept{ return p == Prefix::alm || p == Prefix::aid || p == Prefix::array || p == Prefix::batch || p == Prefix::block; }

} // namespace ate::enums
