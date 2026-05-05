/**
 * @file test_enums.cpp
 * @brief Coverage for prefix.hpp and test_status.hpp enums.
 */
#include <gtest/gtest.h>
#include "ate_parser/enums/prefix.hpp"
#include "ate_parser/enums/test_status.hpp"

using namespace ate::enums;

// ===================================================================
//  Prefix conversions
// ===================================================================

TEST(Prefix, ToPrefixAllKnownStrings) {
    EXPECT_EQ(to_prefix("@A-CAP"),  Prefix::a_cap);
    EXPECT_EQ(to_prefix("@A-DIO"),  Prefix::a_dio);
    EXPECT_EQ(to_prefix("@A-FUS"),  Prefix::a_fus);
    EXPECT_EQ(to_prefix("@A-IND"),  Prefix::a_ind);
    EXPECT_EQ(to_prefix("@A-JUM"),  Prefix::a_jum);
    EXPECT_EQ(to_prefix("@A-MEA"),  Prefix::a_mea);
    EXPECT_EQ(to_prefix("@A-NFE"),  Prefix::a_nfe);
    EXPECT_EQ(to_prefix("@A-NPN"),  Prefix::a_npn);
    EXPECT_EQ(to_prefix("@A-PFE"),  Prefix::a_pfe);
    EXPECT_EQ(to_prefix("@A-PNP"),  Prefix::a_pnp);
    EXPECT_EQ(to_prefix("@A-POT"),  Prefix::a_pot);
    EXPECT_EQ(to_prefix("@A-RES"),  Prefix::a_res);
    EXPECT_EQ(to_prefix("@A-SWI"),  Prefix::a_swi);
    EXPECT_EQ(to_prefix("@A-ZEN"),  Prefix::a_zen);
    EXPECT_EQ(to_prefix("@ALM"),    Prefix::alm);
    EXPECT_EQ(to_prefix("@AID"),    Prefix::aid);
    EXPECT_EQ(to_prefix("@ARRAY"),  Prefix::array);
    EXPECT_EQ(to_prefix("@BATCH"),  Prefix::batch);
    EXPECT_EQ(to_prefix("@BLOCK"),  Prefix::block);
    EXPECT_EQ(to_prefix("@BS-CON"), Prefix::bs_con);
    EXPECT_EQ(to_prefix("@BS-O"),   Prefix::bs_o);
    EXPECT_EQ(to_prefix("@BS-S"),   Prefix::bs_s);
    EXPECT_EQ(to_prefix("@BTEST"),  Prefix::btest);
    EXPECT_EQ(to_prefix("@CCHK"),   Prefix::cchk);
    EXPECT_EQ(to_prefix("@DPIN"),   Prefix::dpin);
    EXPECT_EQ(to_prefix("@D-PLD"),  Prefix::d_pld);
    EXPECT_EQ(to_prefix("@D-T"),    Prefix::d_t);
    EXPECT_EQ(to_prefix("@TS"),     Prefix::ts);
    EXPECT_EQ(to_prefix("@TS-S"),   Prefix::ts_s);
    EXPECT_EQ(to_prefix("@TS-D"),   Prefix::ts_d);
    EXPECT_EQ(to_prefix("@TS-O"),   Prefix::ts_o);
    EXPECT_EQ(to_prefix("@TS-P"),   Prefix::ts_p);
    EXPECT_EQ(to_prefix("@INDICT"), Prefix::indict);
    EXPECT_EQ(to_prefix("@LIM2"),   Prefix::lim2);
    EXPECT_EQ(to_prefix("@LIM3"),   Prefix::lim3);
    EXPECT_EQ(to_prefix("@NETV"),   Prefix::netv);
    EXPECT_EQ(to_prefix("@NODE"),   Prefix::node);
    EXPECT_EQ(to_prefix("@PCHK"),   Prefix::pchk);
    EXPECT_EQ(to_prefix("@PF"),     Prefix::pf);
    EXPECT_EQ(to_prefix("@PIN"),    Prefix::pin);
    EXPECT_EQ(to_prefix("@PRB"),    Prefix::prb);
    EXPECT_EQ(to_prefix("@RETEST"), Prefix::retest);
    EXPECT_EQ(to_prefix("@RPT"),    Prefix::rpt);
    EXPECT_EQ(to_prefix("@TJET"),   Prefix::tjet);
}

TEST(Prefix, ToPrefixUnknown) {
    EXPECT_EQ(to_prefix("@FAKE"), Prefix::unknown);
    EXPECT_EQ(to_prefix(""),      Prefix::unknown);
    EXPECT_EQ(to_prefix("BATCH"), Prefix::unknown);  // missing @
}

TEST(Prefix, RoundTripAllPrefixes) {
    for (auto p : {
        Prefix::a_cap, Prefix::a_dio, Prefix::a_fus, Prefix::a_ind,
        Prefix::a_jum, Prefix::a_mea, Prefix::a_nfe, Prefix::a_npn,
        Prefix::a_pfe, Prefix::a_pnp, Prefix::a_pot, Prefix::a_res,
        Prefix::a_swi, Prefix::a_zen,
        Prefix::alm, Prefix::aid, Prefix::array, Prefix::batch, Prefix::block,
        Prefix::bs_con, Prefix::bs_o, Prefix::bs_s, Prefix::btest,
        Prefix::cchk, Prefix::dpin, Prefix::d_pld, Prefix::d_t,
        Prefix::ts, Prefix::ts_s, Prefix::ts_d, Prefix::ts_o, Prefix::ts_p,
        Prefix::indict, Prefix::lim2, Prefix::lim3, Prefix::netv, Prefix::node,
        Prefix::pchk, Prefix::pf, Prefix::pin, Prefix::prb,
        Prefix::retest, Prefix::rpt, Prefix::tjet, Prefix::unknown,
    }) {
        auto s = to_string(p);
        auto sv = to_string_view(p);
        EXPECT_EQ(s, sv) << "Mismatch for prefix " << static_cast<int>(p);
        if (p != Prefix::unknown) {
            EXPECT_EQ(to_prefix(sv), p) << "Round trip failed for " << s;
        }
    }
}

TEST(Prefix, CategoryPredicates) {
    // Analog
    EXPECT_TRUE(is_analog_test(Prefix::a_cap));
    EXPECT_TRUE(is_analog_test(Prefix::a_zen));
    EXPECT_FALSE(is_analog_test(Prefix::d_t));
    EXPECT_FALSE(is_analog_test(Prefix::batch));

    // Digital
    EXPECT_TRUE(is_digital_test(Prefix::d_t));
    EXPECT_TRUE(is_digital_test(Prefix::cchk));
    EXPECT_TRUE(is_digital_test(Prefix::dpin));
    EXPECT_TRUE(is_digital_test(Prefix::d_pld));
    EXPECT_FALSE(is_digital_test(Prefix::a_res));
    EXPECT_FALSE(is_digital_test(Prefix::ts));

    // Shorts
    EXPECT_TRUE(is_shorts_test(Prefix::ts));
    EXPECT_TRUE(is_shorts_test(Prefix::ts_s));
    EXPECT_TRUE(is_shorts_test(Prefix::ts_d));
    EXPECT_TRUE(is_shorts_test(Prefix::ts_o));
    EXPECT_TRUE(is_shorts_test(Prefix::ts_p));
    EXPECT_FALSE(is_shorts_test(Prefix::d_t));

    // System
    EXPECT_TRUE(is_system_record(Prefix::alm));
    EXPECT_TRUE(is_system_record(Prefix::batch));
    EXPECT_TRUE(is_system_record(Prefix::block));
    EXPECT_TRUE(is_system_record(Prefix::aid));
    EXPECT_TRUE(is_system_record(Prefix::array));
    EXPECT_FALSE(is_system_record(Prefix::btest));
    EXPECT_FALSE(is_system_record(Prefix::a_res));
}

// ===================================================================
//  Test status conversions
// ===================================================================

TEST(TestStatus, AnalogStatusRoundTrip) {
    EXPECT_EQ(to_analog_status(0),  AnalogTestStatus::passed);
    EXPECT_EQ(to_analog_status(1),  AnalogTestStatus::failed);
    EXPECT_EQ(to_analog_status(2),  AnalogTestStatus::failed_compliance_limit);
    EXPECT_EQ(to_analog_status(3),  AnalogTestStatus::failed_detector_timeout);
    EXPECT_EQ(to_analog_status(7),  AnalogTestStatus::failed_general);
    EXPECT_EQ(to_analog_status(11), AnalogTestStatus::aborted_by_operator);
    EXPECT_THROW({ (void)to_analog_status(99); }, std::out_of_range);
}

TEST(TestStatus, AnalogStatusToString) {
    EXPECT_EQ(to_string(AnalogTestStatus::passed),                  "Passed");
    EXPECT_EQ(to_string(AnalogTestStatus::failed),                  "Failed");
    EXPECT_EQ(to_string(AnalogTestStatus::failed_compliance_limit), "Failed (Compliance Limit)");
    EXPECT_EQ(to_string(AnalogTestStatus::failed_detector_timeout), "Failed (Detector Timeout)");
    EXPECT_EQ(to_string(AnalogTestStatus::failed_general),          "Failed (General)");
    EXPECT_EQ(to_string(AnalogTestStatus::aborted_by_operator),     "Aborted by Operator");
}

TEST(TestStatus, DigitalStatusRoundTrip) {
    EXPECT_EQ(to_digital_status(0), DigitalTestStatus::passed);
    EXPECT_EQ(to_digital_status(1), DigitalTestStatus::failed);
    EXPECT_EQ(to_digital_status(5), DigitalTestStatus::crc_related_failure);
    EXPECT_EQ(to_digital_status(7), DigitalTestStatus::fatal_error);
    EXPECT_EQ(to_digital_status(8), DigitalTestStatus::chain_integrity_failure);
    EXPECT_THROW({ (void)to_digital_status(42); }, std::out_of_range);
}

TEST(TestStatus, DigitalStatusToString) {
    EXPECT_EQ(to_string(DigitalTestStatus::passed),                  "Passed");
    EXPECT_EQ(to_string(DigitalTestStatus::failed),                  "Failed");
    EXPECT_EQ(to_string(DigitalTestStatus::crc_related_failure),     "CRC Failure");
    EXPECT_EQ(to_string(DigitalTestStatus::fatal_error),             "Fatal Error");
    EXPECT_EQ(to_string(DigitalTestStatus::chain_integrity_failure), "Chain Integrity Failure");
}

TEST(TestStatus, GenericStatusRoundTrip) {
    EXPECT_EQ(to_generic_status(0),  GenericTestStatus::pass);
    EXPECT_EQ(to_generic_status(1),  GenericTestStatus::fail);
    EXPECT_EQ(to_generic_status(7),  GenericTestStatus::fatal_error);
    EXPECT_EQ(to_generic_status(20), GenericTestStatus::learning_passed);
    EXPECT_THROW({ (void)to_generic_status(55); }, std::out_of_range);
}

TEST(TestStatus, GenericStatusToString) {
    EXPECT_EQ(to_string(GenericTestStatus::pass),            "Pass");
    EXPECT_EQ(to_string(GenericTestStatus::fail),            "Fail");
    EXPECT_EQ(to_string(GenericTestStatus::fatal_error),     "Fatal Error");
    EXPECT_EQ(to_string(GenericTestStatus::learning_passed), "Learning Passed");
}

TEST(TestStatus, DigitalSubstatusRoundTrip) {
    auto flags = decode_digital_substatus(21);  // 1 + 4 + 16
    ASSERT_EQ(flags.size(), 3u);
    EXPECT_EQ(flags[0], 1);
    EXPECT_EQ(flags[1], 4);
    EXPECT_EQ(flags[2], 16);
    EXPECT_EQ(encode_digital_substatus(flags), 21);
}

TEST(TestStatus, DigitalSubstatusZero) {
    auto flags = decode_digital_substatus(0);
    EXPECT_TRUE(flags.empty());
    EXPECT_EQ(encode_digital_substatus(flags), 0);
}

TEST(TestStatus, DigitalSubstatusAllBits) {
    auto flags = decode_digital_substatus(63);  // all 6 bits
    EXPECT_EQ(flags.size(), 6u);
    EXPECT_EQ(encode_digital_substatus(flags), 63);
}
