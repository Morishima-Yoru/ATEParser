#include <gtest/gtest.h>
#include "i3070/containers/LogRecordFactory.hpp"
#include "i3070/records/ShortRecords.hpp"
#include "i3070/utils/JsonKeys.hpp"

using namespace i3070;
using json = nlohmann::json;

template<typename T>
std::unique_ptr<T> createAndParse(enums::LogRecordPrefix prefix, const std::vector<std::string>& fields) {
    auto record = containers::LogRecordFactory::createRecord(prefix);
    record->fromFields(fields);
    return std::unique_ptr<T>(dynamic_cast<T*>(record.release()));
}

TEST(ShortsRecordTest, ShortsTestParsing) {
    auto tsRecord = createAndParse<records::ShortsTestRecord>(
        enums::LogRecordPrefix::TS, {"1", "2", "1", "0", "ShortsTest"});
    
    ASSERT_NE(tsRecord, nullptr);
    EXPECT_EQ(tsRecord->shorts_count, 2);
    
    json j = tsRecord->toJson();
    EXPECT_EQ(j[JSON_KEY_SHORTS_COUNT], 2);
}

TEST(ShortsRecordTest, TsSourceParsing) {
    // @TS-S|shorts|phantoms|source
    auto tss = createAndParse<records::TsSourceRecord>(
        enums::LogRecordPrefix::TS_S, {"2", "0", "N1"});
    ASSERT_NE(tss, nullptr);
    EXPECT_EQ(tss->source_node, "N1");
    
    json j = tss->toJson();
    EXPECT_EQ(j[JSON_KEY_SOURCE_NODE], "N1");
}

TEST(ShortsRecordTest, TsDestinationParsing) {
    auto tsd = createAndParse<records::TsDestinationRecord>(
        enums::LogRecordPrefix::TS_D, {"ignored", "N1", "0.1", "N2", "0.2"});
    ASSERT_NE(tsd, nullptr);
    EXPECT_EQ(tsd->destination_list.size(), 2);
    
    json j = tsd->toJson();
    EXPECT_TRUE(j[JSON_KEY_DESTINATIONS_LIST].is_array());
}

TEST(ShortsRecordTest, TsOpenParsing) {
    auto tso = createAndParse<records::TsOpenRecord>(
        enums::LogRecordPrefix::TS_O, {"N1", "N2", "0.5"});
    ASSERT_NE(tso, nullptr);
    EXPECT_EQ(tso->deviation.value(), 0.5);
    
    json j = tso->toJson();
    EXPECT_DOUBLE_EQ(j[JSON_KEY_DEVIATION], 0.5);
}

TEST(ShortsRecordTest, TsPhantomParsing) {
    // @TS-P|deviation
    auto tsp = createAndParse<records::TsPhantomRecord>(
        enums::LogRecordPrefix::TS_P, {"0.9"});
    ASSERT_NE(tsp, nullptr);
    EXPECT_EQ(tsp->deviation.value(), 0.9);
    
    json j = tsp->toJson();
    EXPECT_DOUBLE_EQ(j[JSON_KEY_DEVIATION], 0.9);
}
