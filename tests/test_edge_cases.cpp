#include <gtest/gtest.h>
#include "i3070/containers/LogRecordFactory.hpp"
#include "i3070/records/AnalogRecords.hpp"
#include "i3070/records/DigitalRecords.hpp"
#include "i3070/utils/JsonKeys.hpp"

using namespace i3070;
using json = nlohmann::json;

template<typename T>
std::unique_ptr<T> createAndParse(enums::LogRecordPrefix prefix, const std::vector<std::string>& fields) {
    auto record = containers::LogRecordFactory::createRecord(prefix);
    record->fromFields(fields);
    return std::unique_ptr<T>(dynamic_cast<T*>(record.release()));
}

// ================= Analog Edge Cases =================

TEST(AnalogEdgeTest, Lim3ZeroNominal) {
    records::Lim3Record lim3(0.0, 10.0, -10.0);
    EXPECT_THROW(lim3.getPercentageDeviation(5.0), std::domain_error);
}

TEST(AnalogEdgeTest, FromFieldsBranching) {
    // Branch 1: size == 4 (Special case in code, likely for record with limit appended but not parsed here)
    // Code says: if size==4, read status(0), measured(1). Don't read designator?
    // Let's verify this behavior.
    auto rec4 = createAndParse<records::ResistorRecord>(
        enums::LogRecordPrefix::A_RES, {"0", "100", "R1", "LimitInfo"});
    
    // If logic holds, designator might be empty/nullopt because code block for size==4 didn't read it?
    // Looking at source: 
    // if (fields.size() == 4) { status=...; measured=...; }
    // else { status=...; measured=...; designator=...; }
    // So for size 4, designator is indeed NOT read.
    EXPECT_FALSE(rec4->subtest_designator.has_value());

    // Branch 2: size != 4 (e.g., 3)
    auto rec3 = createAndParse<records::ResistorRecord>(
        enums::LogRecordPrefix::A_RES, {"0", "100", "R1"});
    EXPECT_EQ(rec3->subtest_designator.value(), "R1");
}

// ================= Digital Edge Cases =================

TEST(DigitalEdgeTest, DevicePinComplexList) {
    // Parsing logic handles nested arrays indicated by backslash in fields
    // Fields: U1, Pin1, Pin2\2, A, B, Pin3
    // i starts at 1 (fields[1]).
    // fields[0] is device name part (U1)
    
    // Let's construct fields such that we trigger the nested array logic.
    // DevicePinRecord expects:
    // fields[0]: DeviceName (or DeviceName\stuff)
    // fields[1..N]: pins.
    
    // Logic:
    // i=1. fields[1]="Pin1". Push "Pin1".
    // i=2. fields[2]="Group\2". 
    //      -> push "Group"
    //      -> read next 2 fields ("A", "B") -> push ["A", "B"]
    //      -> skip 2 fields.
    // i=5. fields[5]="Pin3". Push "Pin3".
    
    std::vector<std::string> fields = {
        "U1", 
        "Pin1", 
        "Group\\2", "A", "B", 
        "Pin3"
    };
    
    auto dpin = createAndParse<records::DevicePinRecord>(enums::LogRecordPrefix::DPIN, fields);
    
    ASSERT_EQ(dpin->node_pin_list.size(), 4);
    EXPECT_EQ(dpin->node_pin_list[0], "Pin1");
    EXPECT_EQ(dpin->node_pin_list[1], "Group");
    
    // Element 2 should be array ["A", "B"]
    EXPECT_TRUE(dpin->node_pin_list[2].is_array());
    EXPECT_EQ(dpin->node_pin_list[2][0], "A");
    EXPECT_EQ(dpin->node_pin_list[2][1], "B");
    
    EXPECT_EQ(dpin->node_pin_list[3], "Pin3");
}

TEST(DigitalEdgeTest, IndictmentParsing) {
    // @INDICT|technique\count|dev1|dev2...|res|cap|ind|model
    // Example: @INDICT|OHM\2|R1|R2|100|0.1|0.01|ModelX
    std::vector<std::string> fields = {
        "OHM\\2", "R1", "R2", "100", "0.1", "0.01", "ModelX"
    };
    
    auto ind = createAndParse<records::IndictmentRecord>(enums::LogRecordPrefix::INDICT, fields);
    ASSERT_NE(ind, nullptr);
    EXPECT_EQ(ind->technique, "OHM");
    ASSERT_EQ(ind->device_list.size(), 2);
    EXPECT_EQ(ind->device_list[0], "R1");
    EXPECT_EQ(ind->device_list[1], "R2");
    
    EXPECT_DOUBLE_EQ(ind->est_resistance.value(), 100.0);
    EXPECT_DOUBLE_EQ(ind->est_capacitance.value(), 0.1);
    EXPECT_DOUBLE_EQ(ind->est_inductance.value(), 0.01);
    EXPECT_EQ(ind->est_model, "ModelX");
    
    json j = ind->toJson();
    EXPECT_EQ(j[JSON_KEY_TECHNIQUE], "OHM");
    EXPECT_EQ(j[JSON_KEY_DEVICE_LIST].size(), 2);
}

TEST(DigitalEdgeTest, PinRecordNoBackslash) {
    // Logic: if no backslash, entire field is count
    auto pin = createAndParse<records::PinRecord>(enums::LogRecordPrefix::PIN, {"5", "P1", "P2"});
    EXPECT_EQ(pin->pin_count.value(), 5);
    EXPECT_EQ(pin->pins.size(), 2);
}
