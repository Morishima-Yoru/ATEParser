#include <gtest/gtest.h>
#include "i3070/containers/LogRecordFactory.hpp"
#include "i3070/records/AnalogRecords.hpp"
#include "i3070/enums/TestStatus.hpp"
#include "i3070/utils/JsonKeys.hpp"

using namespace i3070;
using json = nlohmann::json;

// Helper to create and parse a record
template<typename T>
std::unique_ptr<T> createAndParse(enums::LogRecordPrefix prefix, const std::vector<std::string>& fields) {
    auto record = containers::LogRecordFactory::createRecord(prefix);
    record->fromFields(fields);
    return std::unique_ptr<T>(dynamic_cast<T*>(record.release()));
}

TEST(AnalogRecordTest, ACapParsing) {
    auto capRecord = createAndParse<records::CapacitorRecord>(
        enums::LogRecordPrefix::A_CAP, {"1", "1.246700E+01", "C1"});
    
    ASSERT_NE(capRecord, nullptr);
    EXPECT_EQ(capRecord->test_status, enums::AnalogTestStatus::FAILED);
    ASSERT_TRUE(capRecord->measured_value.has_value());
    EXPECT_DOUBLE_EQ(capRecord->measured_value.value(), 12.467);
    ASSERT_TRUE(capRecord->subtest_designator.has_value());
    EXPECT_EQ(capRecord->subtest_designator.value(), "C1");

    // Helper methods
    EXPECT_FALSE(capRecord->isPassed());
    EXPECT_TRUE(capRecord->isFailed());
    EXPECT_FALSE(capRecord->isAborted());
    EXPECT_EQ(capRecord->getStatusDescription(), "Failed");
    EXPECT_EQ(capRecord->getMeasuredValue(), 12.467);
    EXPECT_EQ(capRecord->getSubtestDesignator(), "C1");

    // JSON
    json j = capRecord->toJson();
    EXPECT_EQ(j[JSON_KEY_TEST_STATUS], 1);
    EXPECT_DOUBLE_EQ(j[JSON_KEY_MEASURED_VALUE], 12.467);
    EXPECT_EQ(j[JSON_KEY_SUBTEST_DESIGNATOR], "C1");
}

TEST(AnalogRecordTest, AResParsing) {
    auto resRecord = createAndParse<records::ResistorRecord>(
        enums::LogRecordPrefix::A_RES, {"0", "100.5", "R1"});

    ASSERT_NE(resRecord, nullptr);
    EXPECT_EQ(resRecord->test_status, enums::AnalogTestStatus::PASSED);
    EXPECT_TRUE(resRecord->isPassed());
    EXPECT_EQ(resRecord->getStatusDescription(), "Passed");

    json j = resRecord->toJson();
    EXPECT_EQ(j[JSON_KEY_TEST_STATUS], 0);
    EXPECT_DOUBLE_EQ(j[JSON_KEY_MEASURED_VALUE], 100.5);
    EXPECT_EQ(j[JSON_KEY_SUBTEST_DESIGNATOR], "R1");
}

// Test other subclasses to ensure they are registered and working
TEST(AnalogRecordTest, OtherSubclassesParsing) {
    auto dio = createAndParse<records::DiodeRecord>(enums::LogRecordPrefix::A_DIO, {"0", "0.7", "D1"});
    EXPECT_NE(dio, nullptr);
    EXPECT_EQ(dio->prefix, enums::LogRecordPrefix::A_DIO);
    
    auto ind = createAndParse<records::InductorRecord>(enums::LogRecordPrefix::A_IND, {"0", "0.01", "L1"});
    EXPECT_NE(ind, nullptr);
    
    auto fus = createAndParse<records::FuseRecord>(enums::LogRecordPrefix::A_FUS, {"0", "0.1", "F1"});
    EXPECT_NE(fus, nullptr);
    
    auto jum = createAndParse<records::JumperRecord>(enums::LogRecordPrefix::A_JUM, {"0", "0.1", "J1"});
    EXPECT_NE(jum, nullptr);
    
    auto mea = createAndParse<records::MeasureRecord>(enums::LogRecordPrefix::A_MEA, {"0", "5.0", "M1"});
    EXPECT_NE(mea, nullptr);
    
    auto nfe = createAndParse<records::NFetRecord>(enums::LogRecordPrefix::A_NFE, {"0", "0.5", "Q1"});
    EXPECT_NE(nfe, nullptr);
    
    auto npn = createAndParse<records::NpnTransistorRecord>(enums::LogRecordPrefix::A_NPN, {"0", "0.6", "Q2"});
    EXPECT_NE(npn, nullptr);
    
    auto pfe = createAndParse<records::PFetRecord>(enums::LogRecordPrefix::A_PFE, {"0", "0.5", "Q3"});
    EXPECT_NE(pfe, nullptr);
    
    auto pnp = createAndParse<records::PnpTransistorRecord>(enums::LogRecordPrefix::A_PNP, {"0", "0.6", "Q4"});
    EXPECT_NE(pnp, nullptr);
    
    auto pot = createAndParse<records::PotentiometerRecord>(enums::LogRecordPrefix::A_POT, {"0", "1000", "VR1"});
    EXPECT_NE(pot, nullptr);
    
    auto swi = createAndParse<records::SwitchRecord>(enums::LogRecordPrefix::A_SWI, {"0", "0.1", "S1"});
    EXPECT_NE(swi, nullptr);
    
    auto zen = createAndParse<records::ZenerDiodeRecord>(enums::LogRecordPrefix::A_ZEN, {"0", "5.1", "Z1"});
    EXPECT_NE(zen, nullptr);
}

TEST(AnalogRecordTest, Lim2Parsing) {
    auto lim2Record = createAndParse<records::Lim2Record>(
        enums::LogRecordPrefix::LIM2, {"10.5", "9.5"});

    ASSERT_NE(lim2Record, nullptr);
    EXPECT_DOUBLE_EQ(lim2Record->high_limit, 10.5);
    EXPECT_DOUBLE_EQ(lim2Record->low_limit, 9.5);
    
    // Helpers
    EXPECT_TRUE(lim2Record->isWithinLimits(10.0));
    EXPECT_FALSE(lim2Record->isWithinLimits(11.0));
    EXPECT_FALSE(lim2Record->isWithinLimits(9.0));
    EXPECT_FALSE(lim2Record->hasNominalValue());
    EXPECT_EQ(lim2Record->getLimitType(), "LIM2");
    EXPECT_DOUBLE_EQ(lim2Record->getRange(), 1.0);
    EXPECT_DOUBLE_EQ(lim2Record->getCenterPoint(), 10.0);

    // JSON
    json j = lim2Record->toJson();
    EXPECT_DOUBLE_EQ(j[JSON_KEY_HIGH_LIMIT], 10.5);
    EXPECT_DOUBLE_EQ(j[JSON_KEY_LOW_LIMIT], 9.5);
}

TEST(AnalogRecordTest, Lim3Parsing) {
    auto lim3Record = createAndParse<records::Lim3Record>(
        enums::LogRecordPrefix::LIM3, {"100.0", "110.0", "90.0"});

    ASSERT_NE(lim3Record, nullptr);
    
    // Helpers
    EXPECT_TRUE(lim3Record->hasNominalValue());
    EXPECT_DOUBLE_EQ(lim3Record->getNominalValue(), 100.0);
    EXPECT_DOUBLE_EQ(lim3Record->getHighLimit(), 110.0);
    EXPECT_DOUBLE_EQ(lim3Record->getLowLimit(), 90.0);
    EXPECT_EQ(lim3Record->getLimitType(), "LIM3");
    
    EXPECT_TRUE(lim3Record->isWithinLimits(100.0));
    EXPECT_TRUE(lim3Record->isWithinLimits(110.0));
    EXPECT_TRUE(lim3Record->isWithinLimits(90.0));
    EXPECT_FALSE(lim3Record->isWithinLimits(110.1));

    EXPECT_DOUBLE_EQ(lim3Record->getPositiveTolerance(), 10.0);
    EXPECT_DOUBLE_EQ(lim3Record->getNegativeTolerance(), 10.0);
    EXPECT_TRUE(lim3Record->hasSymmetricTolerance());
    EXPECT_DOUBLE_EQ(lim3Record->getPercentageDeviation(110.0), 10.0);

    // JSON
    json j = lim3Record->toJson();
    EXPECT_DOUBLE_EQ(j[JSON_KEY_NOMINAL_VALUE], 100.0);
    EXPECT_DOUBLE_EQ(j[JSON_KEY_HIGH_LIMIT], 110.0);
    EXPECT_DOUBLE_EQ(j[JSON_KEY_LOW_LIMIT], 90.0);
}

TEST(AnalogRecordTest, StatusHelpers) {
    EXPECT_EQ(enums::analogTestStatusToString(enums::AnalogTestStatus::PASSED), "Passed");
    EXPECT_EQ(enums::analogTestStatusToString(enums::AnalogTestStatus::FAILED), "Failed");
    EXPECT_EQ(enums::analogTestStatusToString(enums::AnalogTestStatus::FAILED_COMPLIANCE_LIMIT), "Failed (Compliance Limit)");
    
    EXPECT_EQ(enums::intToAnalogTestStatus(0), enums::AnalogTestStatus::PASSED);
    EXPECT_THROW(enums::intToAnalogTestStatus(999), std::out_of_range);
}
