#include "ate_parser/records/digital.hpp"
#include "ate_parser/enums/test_status.hpp"

namespace ate::records {

std::vector<int> DigitalTestRecord::substatus_flags() const {
    return enums::decode_digital_substatus(test_substatus);
}

void DigitalTestRecord::set_substatus_flags(const std::vector<int>& flags) {
    test_substatus = enums::encode_digital_substatus(flags);
}

void DevicePinRecord::add_node_pin(std::string_view node_id, std::string_view device_pin) {
    node_pin_list.push_back({std::string{node_id}, std::string{device_pin}});
}

void DevicePinRecord::add_drive_thru_pair(std::string_view node_id, std::string_view device_id) {
    if (!thru_devnode_list) thru_devnode_list = std::vector<std::string>{};
    thru_devnode_list->emplace_back(node_id);
    thru_devnode_list->emplace_back(device_id);
}

} // namespace ate::records
