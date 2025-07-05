#ifndef KEYSIGHT_LOG_CORE_LOGRECORD_HPP
#define KEYSIGHT_LOG_CORE_LOGRECORD_HPP

#include <string>
#include <vector>
#include "../enums/LogRecordPrefix.hpp"
#include <nlohmann/json.hpp>

using namespace std;

namespace keysight_log {
namespace core {

/**
 * @class LogRecord
 * @brief 所有 log record 的抽象基底類別，提供 prefix 與原始資料欄位。
 */
class LogRecord {
public:
    LogRecord(enums::LogRecordPrefix prefix = enums::LogRecordPrefix::UNKNOWN, 
              const string& raw_prefix = "")
        : prefix(prefix), raw_prefix(raw_prefix), raw_data("") {}
    virtual ~LogRecord() = default;

    // 記錄類型前綴
    enums::LogRecordPrefix prefix;
    // 原始 prefix 字串（如 @PF, @BTEST 等）
    string raw_prefix;
    // 原始 log 字串（可選，供 debug 或還原）
    string raw_data;

    static bool show_raw_field;

    // 取得 prefix 字串
    static string prefixToString(enums::LogRecordPrefix prefix);

    /**
     * @brief 由欄位字串自動解析填入成員（子類需 override）
     * @param fields 由 log 拆解出的欄位字串陣列
     */
    virtual void fromFields(const vector<string>& fields);

    /**
     * @brief 將 record 轉換為 JSON 格式（子類需 override）
     * @return JSON 物件
     */
    virtual nlohmann::json toJson() const;
};

} // namespace core
} // namespace keysight_log

#endif // KEYSIGHT_LOG_CORE_LOGRECORD_HPP 