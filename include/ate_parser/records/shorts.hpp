/**
 * @file shorts.hpp
 * @brief Shorts-test record value types (@TS, @TS-S, @TS-D, @TS-O, @TS-P).
 */
#pragma once

#include "ate_parser/enums/prefix.hpp"
#include "ate_parser/enums/test_status.hpp"
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace ate::records {

struct ShortsTestRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::ts;

    enums::GenericTestStatus test_status     = enums::GenericTestStatus::pass;
    int                      shorts_count    = 0;
    int                      opens_count     = 0;
    int                      phantoms_count  = 0;
    std::string              designator;
};

struct TsSourceRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::ts_s;

    int         shorts_count   = 0;
    int         phantoms_count = 0;
    std::string source_node;
};

struct TsDestinationRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::ts_d;

    std::vector<std::pair<std::string, double>> destination_list;
};

struct TsOpenRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::ts_o;

    std::string           source_node;
    std::string           destination_node;
    std::optional<double> deviation;
};

struct TsPhantomRecord {
    static constexpr enums::Prefix prefix = enums::Prefix::ts_p;

    std::optional<double> deviation;
};

} // namespace ate::records
