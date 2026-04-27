#include "ate_parser/enums/prefix.hpp"
#include <unordered_map>

namespace ate::enums {

namespace {
const std::unordered_map<std::string_view, Prefix>& prefix_table() {
    static const std::unordered_map<std::string_view, Prefix> t = {
        {"@A-CAP", Prefix::a_cap}, {"@A-DIO", Prefix::a_dio}, {"@A-FUS", Prefix::a_fus},
        {"@A-IND", Prefix::a_ind}, {"@A-JUM", Prefix::a_jum}, {"@A-MEA", Prefix::a_mea},
        {"@A-NFE", Prefix::a_nfe}, {"@A-NPN", Prefix::a_npn}, {"@A-PFE", Prefix::a_pfe},
        {"@A-PNP", Prefix::a_pnp}, {"@A-POT", Prefix::a_pot}, {"@A-RES", Prefix::a_res},
        {"@A-SWI", Prefix::a_swi}, {"@A-ZEN", Prefix::a_zen},
        {"@ALM",   Prefix::alm}, {"@AID", Prefix::aid}, {"@ARRAY", Prefix::array},
        {"@BATCH", Prefix::batch}, {"@BLOCK", Prefix::block},
        {"@BS-CON", Prefix::bs_con}, {"@BS-O", Prefix::bs_o}, {"@BS-S", Prefix::bs_s},
        {"@BTEST", Prefix::btest},
        {"@CCHK", Prefix::cchk}, {"@DPIN", Prefix::dpin},
        {"@D-PLD", Prefix::d_pld}, {"@D-T", Prefix::d_t},
        {"@TS", Prefix::ts}, {"@TS-S", Prefix::ts_s}, {"@TS-D", Prefix::ts_d},
        {"@TS-O", Prefix::ts_o}, {"@TS-P", Prefix::ts_p},
        {"@INDICT", Prefix::indict}, {"@LIM2", Prefix::lim2}, {"@LIM3", Prefix::lim3},
        {"@NETV", Prefix::netv}, {"@NODE", Prefix::node}, {"@PCHK", Prefix::pchk},
        {"@PIN", Prefix::pin}, {"@PF", Prefix::pf}, {"@PRB", Prefix::prb},
        {"@RETEST", Prefix::retest}, {"@RPT", Prefix::rpt}, {"@TJET", Prefix::tjet},
    };
    return t;
}
} // namespace

Prefix to_prefix(std::string_view s) noexcept {
    const auto& t = prefix_table();
    auto it = t.find(s);
    return (it != t.end()) ? it->second : Prefix::unknown;
}

std::string to_string(Prefix p) {
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
