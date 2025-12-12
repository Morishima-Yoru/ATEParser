#include <gtest/gtest.h>
#include "i3070/containers/LogRecordFactory.hpp"
#include "i3070/records/SystemRecords.hpp"
#include "i3070/utils/JsonKeys.hpp"

using namespace i3070;
using json = nlohmann::json;

template<typename T>
std::unique_ptr<T> createAndParse(enums::LogRecordPrefix prefix, const std::vector<std::string>& fields) {
    auto record = containers::LogRecordFactory::createRecord(prefix);
    record->fromFields(fields);
    return std::unique_ptr<T>(dynamic_cast<T*>(record.release()));
}

TEST(SystemRecordTest, BatchParsing) {
    auto batchRecord = createAndParse<records::BatchRecord>(
        enums::LogRecordPrefix::BATCH, 
        {"BoardType", "RevA", "123", "1", "ThType", "Step1", "Batch1", "Op1", "Ctrl1", "Plan1", "RevP", "Panel1", "RevPan", "Ver1"}
    );
    
    ASSERT_NE(batchRecord, nullptr);
    EXPECT_EQ(batchRecord->uut_type, "BoardType");
    
    json j = batchRecord->toJson();
    EXPECT_EQ(j[JSON_KEY_UUT_TYPE], "BoardType");
    EXPECT_EQ(j[JSON_KEY_BATCH_ID], "Batch1");
}

TEST(SystemRecordTest, BTestParsing) {
    auto btestRecord = createAndParse<records::BTestRecord>(
        enums::LogRecordPrefix::BTEST, 
        {"SN12345", "0", "20251211143000", "10", "0", "level1", "1", "0", "0", "20251211143010", "Qual", "5", "PanelID"}
    );

    ASSERT_NE(btestRecord, nullptr);
    EXPECT_EQ(btestRecord->board_id, "SN12345");
    
    json j = btestRecord->toJson();
    EXPECT_EQ(j[JSON_KEY_BOARD_ID], "SN12345");
    EXPECT_EQ(j[JSON_KEY_TEST_STATUS], 0);
}

TEST(SystemRecordTest, BlockParsing) {
    auto blockRecord = createAndParse<records::BlockRecord>(
        enums::LogRecordPrefix::BLOCK, {"MainBlock", "1"});
    
    ASSERT_NE(blockRecord, nullptr);
    
    json j = blockRecord->toJson();
    EXPECT_EQ(j[JSON_KEY_BLOCK_DESIGNATOR], "MainBlock");
    EXPECT_EQ(j[JSON_KEY_BLOCK_STATUS], 1);
}

TEST(SystemRecordTest, AlarmParsing) {
    // @ALM|alarm type|alarm status|datetime|board type|board rev|alarm limit|detected value|controller|testhead number
    auto alm = createAndParse<records::AlarmRecord>(
        enums::LogRecordPrefix::ALM, 
        {"1", "1", "2025", "Board", "Rev", "100", "90", "Ctrl", "2"}
    );
    ASSERT_NE(alm, nullptr);
    EXPECT_EQ(alm->alarm_type, 1);
    EXPECT_TRUE(alm->alarm_status);
    
    json j = alm->toJson();
    EXPECT_EQ(j[JSON_KEY_ALARM_TYPE], 1);
    EXPECT_TRUE(j[JSON_KEY_ALARM_STATUS]);
}

TEST(SystemRecordTest, AlarmBoardParsing) {
    // @AID|datetime|serial (AID maps to AlarmBoardRecord in factory/system records usually)
    auto ab = createAndParse<records::AlarmBoardRecord>(
        enums::LogRecordPrefix::AID, {"2025", "SN123"});
    ASSERT_NE(ab, nullptr);
    EXPECT_EQ(ab->serial, "SN123");
    
    json j = ab->toJson();
    EXPECT_EQ(j[JSON_KEY_SERIAL_NUMBER], "SN123");
}

TEST(SystemRecordTest, ArrayParsing) {
    // @ARRAY|designator|status|failure count|samples
    auto arr = createAndParse<records::ArrayRecord>(
        enums::LogRecordPrefix::ARRAY, {"U1", "1", "5", "100"});
    ASSERT_NE(arr, nullptr);
    EXPECT_EQ(arr->designator, "U1");
    EXPECT_EQ(arr->status, 1);
    
    json j = arr->toJson();
    EXPECT_EQ(j[JSON_KEY_DESIGNATOR], "U1");
    EXPECT_EQ(j[JSON_KEY_FAILURE_COUNT], 5);
}

TEST(SystemRecordTest, NetVerifyParsing) {
    // @NETV|datetime|test system|repair system|source
    auto nv = createAndParse<records::NetVerifyRecord>(
        enums::LogRecordPrefix::NETV, {"2025", "Sys1", "Rep1", "1"});
    ASSERT_NE(nv, nullptr);
    EXPECT_TRUE(nv->source);
    
    json j = nv->toJson();
    EXPECT_EQ(j[JSON_KEY_TEST_SYSTEM], "Sys1");
    EXPECT_TRUE(j[JSON_KEY_SOURCE]);
}

TEST(SystemRecordTest, NodeListParsing) {
    auto nodeRecord = createAndParse<records::NodeListRecord>(
        enums::LogRecordPrefix::NODE, {"\\2", "Node1", "Node2"});
    ASSERT_NE(nodeRecord, nullptr);
    EXPECT_EQ(nodeRecord->count, 2);
    
    json j = nodeRecord->toJson();
    EXPECT_EQ(j[JSON_KEY_COUNT], 2);
    EXPECT_TRUE(j[JSON_KEY_NODES].is_array());
    EXPECT_EQ(j[JSON_KEY_NODES].size(), 2);
}

TEST(SystemRecordTest, ReportParsing) {
    auto rpt = createAndParse<records::ReportRecord>(
        enums::LogRecordPrefix::RPT, {"MsgPart1", "MsgPart2"});
    ASSERT_NE(rpt, nullptr);
    EXPECT_EQ(rpt->message, "MsgPart1|MsgPart2");
    
    json j = rpt->toJson();
    EXPECT_EQ(j[JSON_KEY_MESSAGE], "MsgPart1|MsgPart2");
}

TEST(SystemRecordTest, RetestParsing) {
    auto ret = createAndParse<records::RetestRecord>(
        enums::LogRecordPrefix::RETEST, {"2025"});
    ASSERT_NE(ret, nullptr);
    
    json j = ret->toJson();
    EXPECT_EQ(j[JSON_KEY_DATETIME], "2025");
}

TEST(SystemRecordTest, PFParsing) {
    // @PF|designator|test status|total pins
    auto pf = createAndParse<records::PFRecord>(
        enums::LogRecordPrefix::PF, {"U1", "1", "10"});
    ASSERT_NE(pf, nullptr);
    EXPECT_EQ(pf->designator, "U1");
    
    json j = pf->toJson();
    EXPECT_EQ(j[JSON_KEY_DESIGNATOR], "U1");
    EXPECT_EQ(j[JSON_KEY_TOTAL_PINS], 10);
}

TEST(SystemRecordTest, BsShortParsing) {
    // @BS-S|cause\count|node...
    auto bss = createAndParse<records::BsShortRecord>(
        enums::LogRecordPrefix::BS_S, {"Cause\\2", "N1", "N2"});
    ASSERT_NE(bss, nullptr);
    EXPECT_EQ(bss->cause, "Cause");
    EXPECT_EQ(bss->shorts_count, 2);
    
    json j = bss->toJson();
    EXPECT_EQ(j[JSON_KEY_CAUSE], "Cause");
    EXPECT_EQ(j[JSON_KEY_SHORTS_COUNT], 2);
}

TEST(SystemRecordTest, BsOpenParsing) {
    // @BS-O|dev1|pin1|dev2|pin2
    auto bso = createAndParse<records::BsOpenRecord>(
        enums::LogRecordPrefix::BS_O, {"U1", "1", "U2", "2"});
    ASSERT_NE(bso, nullptr);
    EXPECT_EQ(bso->first_device_name, "U1");
    
    json j = bso->toJson();
    EXPECT_EQ(j[JSON_KEY_FIRST_DEVICE_NAME], "U1");
}

TEST(SystemRecordTest, BoundaryScanParsing) {
    // @BS-CON|desig|status|shorts|opens
    auto bs = createAndParse<records::BoundaryScanRecord>(
        enums::LogRecordPrefix::BS_CON, {"BS1", "1", "2", "3"});
    ASSERT_NE(bs, nullptr);
    EXPECT_EQ(bs->test_designator, "BS1");
    
    json j = bs->toJson();
    EXPECT_EQ(j[JSON_KEY_TEST_DESIGNATOR], "BS1");
    EXPECT_EQ(j[JSON_KEY_SHORTS_COUNT], 2);
}
