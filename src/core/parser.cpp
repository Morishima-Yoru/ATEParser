#include "ate_parser/core/parser.hpp"
#include "ate_parser/utils/errors.hpp"
#include "ate_parser/utils/logging.hpp"

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace ate::core {
namespace {

inline void trim_in_place(std::string& s) {
    if (s.empty()) return;
    constexpr const char* ws = " \t\r\n";
    auto start = s.find_first_not_of(ws);
    if (start == std::string::npos) { s.clear(); return; }
    auto end = s.find_last_not_of(ws);
    s.erase(end + 1);
    s.erase(0, start);
}

inline bool starts_with(std::string_view text, std::string_view prefix) noexcept {
    return text.size() >= prefix.size() && text.compare(0, prefix.size(), prefix) == 0;
}

inline bool is_analog_test_text(std::string_view t) noexcept { return starts_with(t, "@A-"); }

/// Brace-aware splitter -- ignores delimiters that occur within `{...}` regions.
std::vector<std::string_view> split_fields(std::string_view sv, char delim, std::size_t start = 0) {
    std::vector<std::string_view> out;
    if (sv.empty() || start >= sv.size()) return out;
    out.reserve(16);
    std::size_t pos = start;
    std::size_t field_start = pos;
    int brace_depth = 0;
    while (pos < sv.size()) {
        const char c = sv[pos];
        if (c == '{')                         ++brace_depth;
        else if (c == '}' && brace_depth > 0) --brace_depth;
        else if (c == delim && brace_depth == 0) {
            out.emplace_back(sv.substr(field_start, pos - field_start));
            field_start = pos + 1;
        }
        ++pos;
    }
    if (field_start < sv.size()) out.emplace_back(sv.substr(field_start));
    return out;
}

std::size_t find_matching_brace(std::string_view text, std::size_t open_pos) {
    if (open_pos >= text.size() || text[open_pos] != '{') return std::string::npos;
    int depth = 1;
    for (std::size_t i = open_pos + 1; i < text.size(); ++i) {
        if (text[i] == '{') ++depth;
        else if (text[i] == '}' && --depth == 0) return i;
    }
    return std::string::npos;
}

// ---- Specialised parseFields helpers ----

ParsedFields parse_rpt_fields(std::string_view t) {
    auto tilde = t.find('~');
    if (tilde == std::string_view::npos) {
        auto fields = split_fields(t, '|', std::min<std::size_t>(t.size(), 5));  // skip "@RPT"
        return {"@RPT", std::move(fields)};
    }
    auto pipe = t.find('|', tilde);
    if (pipe == std::string_view::npos) {
        throw MalformedRecordError("Invalid @RPT format: missing | after literal length");
    }
    int literal_length = 0;
    {
        auto len_sv = t.substr(tilde + 1, pipe - tilde - 1);
        auto [ptr, ec] = std::from_chars(len_sv.data(), len_sv.data() + len_sv.size(), literal_length);
        if (ec != std::errc{} || ptr != len_sv.data() + len_sv.size()) {
            throw MalformedRecordError(("Invalid @RPT literal length: " + std::string{len_sv}).c_str());
        }
    }
    const std::size_t lit_start = pipe + 1;
    if (lit_start + static_cast<std::size_t>(literal_length) > t.size()) {
        throw MalformedRecordError("Invalid @RPT format: literal exceeds record size");
    }
    std::vector<std::string_view> fields;
    fields.reserve(8);
    fields.emplace_back(t.substr(lit_start, literal_length));
    const std::size_t rest = lit_start + literal_length;
    if (rest < t.size()) {
        auto more = split_fields(t.substr(rest), '|');
        fields.insert(fields.end(),
                      std::make_move_iterator(more.begin()),
                      std::make_move_iterator(more.end()));
    }
    return {"@RPT", std::move(fields)};
}

ParsedFields parse_pin_fields(std::string_view t) {
    auto bs = t.find('\\');
    if (bs == std::string_view::npos) {
        return {"@PIN", split_fields(t, '|', std::min<std::size_t>(t.size(), 5))};
    }
    auto pipe = t.find('|', bs);
    if (pipe == std::string_view::npos) {
        throw MalformedRecordError("Invalid @PIN format: missing | after count");
    }
    std::vector<std::string_view> fields;
    fields.reserve(8);
    fields.emplace_back(t.substr(bs, pipe - bs));   // "\count"
    if (pipe + 1 < t.size()) {
        auto more = split_fields(t.substr(pipe + 1), '|');
        fields.insert(fields.end(),
                      std::make_move_iterator(more.begin()),
                      std::make_move_iterator(more.end()));
    }
    return {"@PIN", std::move(fields)};
}

ParsedFields parse_tsd_fields(std::string_view t) {
    auto bs = t.find('\\');
    if (bs == std::string_view::npos) {
        auto pipe = t.find('|');
        if (pipe == std::string_view::npos) return {"@TS-D", {}};
        return {"@TS-D", split_fields(t, '|', pipe + 1)};
    }
    auto pipe = t.find('|', bs);
    if (pipe == std::string_view::npos) {
        throw MalformedRecordError("Invalid @TS-D format: missing | after count");
    }
    std::vector<std::string_view> fields;
    fields.reserve(8);
    fields.emplace_back(t.substr(bs, pipe - bs));
    if (pipe + 1 < t.size()) {
        auto more = split_fields(t, '|', pipe + 1);
        fields.insert(fields.end(),
                      std::make_move_iterator(more.begin()),
                      std::make_move_iterator(more.end()));
    }
    return {"@TS-D", std::move(fields)};
}

ParsedFields parse_analog_fields(std::string_view t) {
    auto sep = t.find('|');
    if (sep == std::string_view::npos) return {std::string{t}, {}};
    std::string prefix{t.substr(0, sep)};
    std::vector<std::string_view> fields;
    fields.reserve(3);

    std::size_t start = sep + 1;
    auto next = t.find('|', start);
    if (next == std::string_view::npos) {
        fields.emplace_back(t.substr(start));
        fields.emplace_back();   // measured_value
        fields.emplace_back();   // subtest_designator
        return {std::move(prefix), std::move(fields)};
    }
    fields.emplace_back(t.substr(start, next - start));   // test_status
    start = next + 1;

    auto brace = t.find('{', start);
    auto pipe  = t.find('|', start);
    if (pipe == std::string_view::npos || (brace != std::string_view::npos && brace < pipe)) {
        // No subtest_designator
        const std::size_t n = (brace == std::string_view::npos) ? std::string_view::npos : brace - start;
        fields.emplace_back(t.substr(start, n));
        fields.emplace_back();
        return {std::move(prefix), std::move(fields)};
    }
    fields.emplace_back(t.substr(start, pipe - start));
    start = pipe + 1;
    brace = t.find('{', start);
    fields.emplace_back(t.substr(start, brace == std::string_view::npos ? std::string_view::npos : brace - start));
    return {std::move(prefix), std::move(fields)};
}

void parse_into_node(std::string_view text, RecordNode& parent, bool keep_raw) {
    std::size_t pos = 0;
    while (pos < text.size()) {
        auto open = text.find('{', pos);
        if (open == std::string_view::npos) break;
        auto close = find_matching_brace(text, open);
        if (close == std::string_view::npos) {
            throw MalformedRecordError("Unbalanced braces in record stream");
        }
        std::string_view body = text.substr(open + 1, close - open - 1);
        const std::size_t first_nested = body.find('{');
        std::string_view flat = body.substr(0, first_nested);

        auto pf = Parser::parse_fields(flat);
        trim_in_place(pf.prefix);
        if (pf.prefix.empty()) {
            pos = close + 1;
            continue;
        }
        auto prefix = enums::to_prefix(pf.prefix);

        RecordNode node;
        node.record    = make_record(prefix);
        node.raw_prefix = pf.prefix;
        if (keep_raw) node.raw_data = std::string{flat};
        parse_into(node.record, pf.fields);

        if (first_nested != std::string_view::npos) {
            parse_into_node(body.substr(first_nested), node, keep_raw);
        }
        parent.children.push_back(std::move(node));
        pos = close + 1;
    }
}

} // namespace

ParsedFields Parser::parse_fields(std::string_view t) {
    if (t.empty()) return {"", {}};
    if (starts_with(t, "@RPT"))    return parse_rpt_fields(t);
    if (starts_with(t, "@PIN"))    return parse_pin_fields(t);
    if (starts_with(t, "@TS-D"))   return parse_tsd_fields(t);
    if (is_analog_test_text(t))    return parse_analog_fields(t);

    auto sep = t.find('|');
    if (sep == std::string_view::npos) return {std::string{t}, {}};

    std::string prefix{t.substr(0, sep)};
    std::vector<std::string_view> fields = split_fields(t, '|', sep + 1);
    return {std::move(prefix), std::move(fields)};
}

RecordNode Parser::parse(std::string_view log_text, bool keep_raw) const {
    RecordNode root;  // synthetic monostate root
    if (log_text.empty()) {
        throw IntegrityError("empty log content");
    }

    auto batch_pos = log_text.find("{@BATCH");
    if (batch_pos == std::string_view::npos) {
        throw IntegrityError("failed integrity: no @BATCH record found");
    }

    // Validate the first record really is @BATCH (not @BATCH<something>).
    const std::size_t scan_from = batch_pos + 1;
    auto pipe = log_text.find('|', scan_from);
    auto brace = log_text.find('{', scan_from);
    auto end   = std::min(pipe, brace);
    std::string first_prefix{log_text.substr(scan_from, end - scan_from)};
    trim_in_place(first_prefix);
    if (first_prefix != "@BATCH") {
        throw IntegrityError("failed integrity: first record is not @BATCH");
    }

    parse_into_node(log_text.substr(batch_pos), root, keep_raw);
    return root;
}

// =====================================================================
// C ABI for embedders
// =====================================================================

namespace {

char* dup_cstr(const std::string& s) {
#ifdef _WIN32
    return _strdup(s.c_str());
#else
    return strdup(s.c_str());
#endif
}

int parse_string_impl(const char* log_content, const char* dst_filepath,
                      int indent, int keep_raw, char** out_json_str) {
    if (!log_content || !out_json_str) return static_cast<int>(ParseResult::input_error);
    *out_json_str = nullptr;
    try {
        Parser parser;
        auto tree = parser.parse(log_content, keep_raw != 0);
        // Emit the first child (the @BATCH record) at the JSON root, mirroring legacy shape.
        nlohmann::json j;
        if (!tree.children.empty()) j = to_json(tree.children.front(), keep_raw != 0);
        else                        j = nlohmann::json::object();

        // When indent < 0, emit the most compact JSON form (no whitespace between
        // tokens). Calling dump() with no arguments uses nlohmann::json's default
        // compact separators which is the desired hot-path representation.
        const std::string dumped = (indent < 0) ? j.dump() : j.dump(indent);
        *out_json_str = dup_cstr(dumped);
        if (!*out_json_str) return static_cast<int>(ParseResult::output_error);

        if (dst_filepath && std::strlen(dst_filepath) > 0) {
            std::ofstream ofs(dst_filepath);
            if (!ofs) return static_cast<int>(ParseResult::output_error);
            ofs << dumped;
        }
        return static_cast<int>(ParseResult::success);
    } catch (const IntegrityError&) {
        return static_cast<int>(ParseResult::failed_integrity);
    } catch (const ParseError&) {
        return static_cast<int>(ParseResult::parse_error);
    } catch (const std::exception&) {
        return static_cast<int>(ParseResult::parse_error);
    } catch (...) {
        return static_cast<int>(ParseResult::parse_error);
    }
}

} // namespace

extern "C" ATE_API int ate_parse_file(const char* log_filepath, const char* dst_filepath,
                                      int indent, int keep_raw, char** out_json_str) {
    if (!log_filepath || !out_json_str) return static_cast<int>(ParseResult::input_error);
    *out_json_str = nullptr;
    std::ifstream ifs(log_filepath);
    if (!ifs) return static_cast<int>(ParseResult::input_error);
    std::stringstream buf;
    buf << ifs.rdbuf();
    const std::string text = buf.str();
    if (text.empty()) return static_cast<int>(ParseResult::input_error);
    return parse_string_impl(text.c_str(), dst_filepath, indent, keep_raw, out_json_str);
}

extern "C" ATE_API int ate_parse_log_string(const char* log_content, const char* dst_filepath,
                                            int indent, int keep_raw, char** out_json_str) {
    return parse_string_impl(log_content, dst_filepath, indent, keep_raw, out_json_str);
}

extern "C" ATE_API void ate_free_string(char* ptr) {
    if (ptr) std::free(ptr);
}

} // namespace ate::core
