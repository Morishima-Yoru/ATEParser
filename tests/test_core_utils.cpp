#include <gtest/gtest.h>
#include "i3070/utils/SafeConversion.hpp"
#include "i3070/core/FieldValue.hpp"
#include "i3070/containers/LogRecordContainer.hpp"
#include "i3070/containers/LogRecordFactory.hpp"
#include "i3070/enums/LogRecordPrefix.hpp"
#include "i3070/enums/FieldType.hpp"
#include "i3070/core/LogRecord.hpp"

using namespace i3070;
using namespace i3070::core;
using namespace i3070::containers;
using namespace i3070::enums;

// ================= SafeConversion Tests =================

TEST(SafeConversionTest, SafeStoi) {
    EXPECT_EQ(safeStoi("123", "test", "type", 0), 123);
    EXPECT_EQ(safeStoi("-456", "test", "type", 0), -456);
    
    EXPECT_THROW(safeStoi("", "test", "type", 0), std::invalid_argument);
    EXPECT_THROW(safeStoi("abc", "test", "type", 0), std::invalid_argument);
    // safeStoi does NOT use default_value on error, it throws.
}

TEST(SafeConversionTest, SafeStod) {
    EXPECT_DOUBLE_EQ(safeStod("123.456", "test", "type", 0.0), 123.456);
    EXPECT_DOUBLE_EQ(safeStod("-1.23E+2", "test", "type", 0.0), -123.0);
    
    EXPECT_THROW(safeStod("", "test", "type", 0.0), std::invalid_argument);
    EXPECT_THROW(safeStod("xyz", "test", "type", 0.0), std::invalid_argument);
}

TEST(SafeConversionTest, SafeStob) {
    EXPECT_TRUE(safeStob("1", "test", "type", false));
    EXPECT_TRUE(safeStob("Y", "test", "type", false));
    EXPECT_TRUE(safeStob("YES", "test", "type", false));
    EXPECT_TRUE(safeStob("True", "test", "type", false));
    
    EXPECT_FALSE(safeStob("0", "test", "type", true));
    EXPECT_FALSE(safeStob("N", "test", "type", true));
    EXPECT_FALSE(safeStob("NO", "test", "type", true));
    EXPECT_FALSE(safeStob("False", "test", "type", true));
    
    // Empty uses default
    EXPECT_TRUE(safeStob("", "test", "type", true));
    EXPECT_FALSE(safeStob("", "test", "type", false));
    
    // Invalid uses default (and logs error but we don't check stderr here easily)
    EXPECT_TRUE(safeStob("invalid", "test", "type", true));
}

// ================= FieldValue Tests =================

TEST(FieldValueTest, ToString) {
    FieldValue b = true;
    EXPECT_EQ(fieldValueToString(b), "1");
    b = false;
    EXPECT_EQ(fieldValueToString(b), "0");
    
    FieldValue i = 123;
    EXPECT_EQ(fieldValueToString(i), "123");
    
    FieldValue d = 1.23;
    // Implementation uses "%.6E", so check against that format
    char buf[32];
    snprintf(buf, sizeof(buf), "%.6E", 1.23);
    EXPECT_EQ(fieldValueToString(d), std::string(buf));
    
    FieldValue s = std::string("text");
    EXPECT_EQ(fieldValueToString(s), "text");
    
    // Test getFieldValueType
    EXPECT_EQ(getFieldValueType(b), 'b');
    EXPECT_EQ(getFieldValueType(i), 'i');
    EXPECT_EQ(getFieldValueType(d), 'f');
    EXPECT_EQ(getFieldValueType(s), 's');
}

// ================= LogRecordContainer Tests =================

TEST(LogRecordContainerTest, BasicUsage) {
    auto rec = std::make_unique<LogRecord>(LogRecordPrefix::BATCH);
    LogRecordContainer container(std::move(rec));
    
    EXPECT_NE(container.getRecord(), nullptr);
    EXPECT_EQ(container.getRecord()->prefix, LogRecordPrefix::BATCH);
    EXPECT_FALSE(container.hasSubrecords());
    
    auto sub = std::make_unique<LogRecordContainer>(std::make_unique<LogRecord>(LogRecordPrefix::BLOCK));
    container.addSubrecord(std::move(sub));
    
    EXPECT_TRUE(container.hasSubrecords());
    EXPECT_EQ(container.getSubrecords().size(), 1);
    EXPECT_EQ(container.getSubrecords()[0]->getRecord()->prefix, LogRecordPrefix::BLOCK);
    
    container.clearSubrecords();
    EXPECT_FALSE(container.hasSubrecords());
}

// ================= LogRecordFactory Tests =================

TEST(LogRecordFactoryTest, Creation) {
    auto r1 = LogRecordFactory::createRecord(LogRecordPrefix::A_RES);
    EXPECT_NE(dynamic_cast<records::ResistorRecord*>(r1.get()), nullptr);
    
    auto r2 = LogRecordFactory::createRecord(LogRecordPrefix::D_T);
    EXPECT_NE(dynamic_cast<records::DigitalTestRecord*>(r2.get()), nullptr);
    
    auto r3 = LogRecordFactory::createRecord(LogRecordPrefix::UNKNOWN);
    EXPECT_EQ(r3->prefix, LogRecordPrefix::UNKNOWN); // Default constructs base LogRecord which defaults to UNKNOWN?
    // LogRecord default ctor: prefix(LogRecordPrefix::UNKNOWN)
}

// ================= LogRecordPrefix Tests =================

TEST(LogRecordPrefixTest, Conversions) {
    // Test a sample of conversions
    EXPECT_EQ(stringToPrefix("@A-RES"), LogRecordPrefix::A_RES);
    EXPECT_EQ(stringToPrefix("@BATCH"), LogRecordPrefix::BATCH);
    EXPECT_EQ(stringToPrefix("@UNKNOWN_THING"), LogRecordPrefix::UNKNOWN);
    
    EXPECT_EQ(prefixToString(LogRecordPrefix::A_RES), "@A-RES");
    EXPECT_EQ(prefixToString(LogRecordPrefix::BATCH), "@BATCH");
    EXPECT_EQ(prefixToString(LogRecordPrefix::UNKNOWN), "@UNKNOWN"); // Verify unknown behavior
}

TEST(LogRecordPrefixTest, AllPrefixes) {
    // Iterate through a representative set of prefixes to ensure switch coverage
    std::vector<LogRecordPrefix> prefixes = {
        LogRecordPrefix::A_CAP, LogRecordPrefix::A_DIO, LogRecordPrefix::A_FUS, 
        LogRecordPrefix::A_IND, LogRecordPrefix::A_JUM, LogRecordPrefix::A_MEA,
        LogRecordPrefix::A_NFE, LogRecordPrefix::A_NPN, LogRecordPrefix::A_PFE,
        LogRecordPrefix::A_PNP, LogRecordPrefix::A_POT, LogRecordPrefix::A_RES,
        LogRecordPrefix::A_SWI, LogRecordPrefix::A_ZEN,
        LogRecordPrefix::ALM, LogRecordPrefix::AID, LogRecordPrefix::ARRAY,
        LogRecordPrefix::BATCH, LogRecordPrefix::BLOCK,
        LogRecordPrefix::BS_CON, LogRecordPrefix::BS_O, LogRecordPrefix::BS_S,
        LogRecordPrefix::BTEST,
        LogRecordPrefix::CCHK, LogRecordPrefix::DPIN, LogRecordPrefix::D_PLD,
        LogRecordPrefix::D_T,
        LogRecordPrefix::TS, LogRecordPrefix::TS_S, LogRecordPrefix::TS_D,
        LogRecordPrefix::TS_O, LogRecordPrefix::TS_P,
        LogRecordPrefix::INDICT, LogRecordPrefix::NETV, LogRecordPrefix::NODE,
        LogRecordPrefix::PCHK, LogRecordPrefix::PIN, LogRecordPrefix::PF,
        LogRecordPrefix::PRB, LogRecordPrefix::RETEST, LogRecordPrefix::RPT,
        LogRecordPrefix::TJET,
        LogRecordPrefix::LIM2, LogRecordPrefix::LIM3
    };

    for (auto p : prefixes) {
        std::string s = prefixToString(p);
        EXPECT_FALSE(s.empty());
        EXPECT_NE(s, "unknown"); // Should have a valid string mapping
        EXPECT_EQ(stringToPrefix(s), p); // Should round-trip
    }
}

TEST(LogRecordPrefixTest, Categories) {
    EXPECT_TRUE(isAnalogTestPrefix(LogRecordPrefix::A_RES));
    EXPECT_FALSE(isAnalogTestPrefix(LogRecordPrefix::BATCH));
    
    EXPECT_TRUE(isDigitalTestPrefix(LogRecordPrefix::D_T));
    EXPECT_FALSE(isDigitalTestPrefix(LogRecordPrefix::A_RES));
    
    EXPECT_TRUE(isShortsTestPrefix(LogRecordPrefix::TS));
    EXPECT_FALSE(isShortsTestPrefix(LogRecordPrefix::D_T));
    
    EXPECT_TRUE(isSystemRecordPrefix(LogRecordPrefix::BATCH));
    EXPECT_FALSE(isSystemRecordPrefix(LogRecordPrefix::TS));
}

// ================= FieldType Tests =================

TEST(FieldTypeTest, StringToEnum) {
    EXPECT_EQ(stringToFieldType("bool"), FieldType::BOOL);
    EXPECT_EQ(stringToFieldType("boolean"), FieldType::BOOL);
    EXPECT_EQ(stringToFieldType("fp"), FieldType::FP);
    EXPECT_EQ(stringToFieldType("double"), FieldType::FP);
    EXPECT_EQ(stringToFieldType("int"), FieldType::INT);
    EXPECT_EQ(stringToFieldType("integer"), FieldType::INT);
    EXPECT_EQ(stringToFieldType("str"), FieldType::STR);
    EXPECT_EQ(stringToFieldType("string"), FieldType::STR);
    EXPECT_EQ(stringToFieldType("list"), FieldType::LIST);
    EXPECT_EQ(stringToFieldType("literal"), FieldType::LITERAL);
    EXPECT_EQ(stringToFieldType("invalid"), FieldType::UNKNOWN);
}

TEST(FieldTypeTest, EnumToString) {
    EXPECT_EQ(fieldTypeToString(FieldType::BOOL), "bool");
    EXPECT_EQ(fieldTypeToString(FieldType::FP), "fp");
    EXPECT_EQ(fieldTypeToString(FieldType::INT), "int");
    EXPECT_EQ(fieldTypeToString(FieldType::STR), "str");
    EXPECT_EQ(fieldTypeToString(FieldType::LIST), "list");
    EXPECT_EQ(fieldTypeToString(FieldType::LITERAL), "literal");
    EXPECT_EQ(fieldTypeToString(FieldType::UNKNOWN), "unknown");
}

TEST(FieldTypeTest, ParseValues) {
    // Bool
    EXPECT_TRUE(std::get<bool>(*parseStringToFieldValue("1", FieldType::BOOL)));
    EXPECT_TRUE(std::get<bool>(*parseStringToFieldValue("true", FieldType::BOOL)));
    EXPECT_FALSE(std::get<bool>(*parseStringToFieldValue("0", FieldType::BOOL)));
    EXPECT_FALSE(std::get<bool>(*parseStringToFieldValue("false", FieldType::BOOL)));
    EXPECT_FALSE(parseStringToFieldValue("invalid", FieldType::BOOL).has_value());

    // Int
    EXPECT_EQ(std::get<int>(*parseStringToFieldValue("123", FieldType::INT)), 123);
    EXPECT_EQ(std::get<int>(*parseStringToFieldValue("-456", FieldType::INT)), -456);
    EXPECT_FALSE(parseStringToFieldValue("abc", FieldType::INT).has_value());

    // Float
    EXPECT_DOUBLE_EQ(std::get<double>(*parseStringToFieldValue("123.45", FieldType::FP)), 123.45);
    EXPECT_DOUBLE_EQ(std::get<double>(*parseStringToFieldValue("1.23E+2", FieldType::FP)), 123.0);
    EXPECT_FALSE(parseStringToFieldValue("xyz", FieldType::FP).has_value());

    // String
    EXPECT_EQ(std::get<std::string>(*parseStringToFieldValue("hello", FieldType::STR)), "hello");
}

// ================= LogRecord Tests =================

TEST(LogRecordTest, ToJsonBase) {
    bool old_show = LogRecord::show_raw_field;
    LogRecord::show_raw_field = true;
    
    LogRecord rec(LogRecordPrefix::BATCH);
    rec.raw_data = "some|raw|content";
    
    nlohmann::json j = rec.toJson();
    EXPECT_EQ(j["prefix"], "@BATCH");
    EXPECT_EQ(j["raw"], "some|raw|content");
    
    LogRecord::show_raw_field = old_show;
}
