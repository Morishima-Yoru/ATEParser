/**
 * @file ate_parser.hpp
 * @brief Umbrella header. Include this and you are ready to go.
 */
#pragma once

#include "ate_parser/core/parser.hpp"
#include "ate_parser/core/record.hpp"
#include "ate_parser/core/record_tree.hpp"
#include "ate_parser/enums/field_type.hpp"
#include "ate_parser/enums/prefix.hpp"
#include "ate_parser/enums/test_status.hpp"
#include "ate_parser/records/analog.hpp"
#include "ate_parser/records/digital.hpp"
#include "ate_parser/records/shorts.hpp"
#include "ate_parser/records/system.hpp"
#include "ate_parser/utils/errors.hpp"
#include "ate_parser/utils/json_keys.hpp"
#include "ate_parser/utils/logging.hpp"
#include "ate_parser/utils/safe_conversion.hpp"
