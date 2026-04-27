/**
 * @file errors.hpp
 * @brief Project-wide exception hierarchy with attached stack traces.
 *
 * All exceptions inherit from `cpptrace::exception_with_message`, which
 * captures a full, symbolized stack trace at construction time. Catch
 * sites can call `.trace().print()` for a Python-like traceback.
 */
#pragma once

#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include <string>
#include <string_view>

namespace ate {

/// Generic, traceable parse error -- root of the hierarchy.
class ParseError : public cpptrace::exception_with_message {
public:
    using cpptrace::exception_with_message::exception_with_message;
};

/// Failed string -> number / boolean conversion.
class ConversionError : public ParseError {
public:
    ConversionError(std::string_view raw,
                    std::string_view target_type,
                    std::string_view field_name,
                    std::string_view record_type)
        : ParseError(make_message(raw, target_type, field_name, record_type)),
          raw_(raw), target_type_(target_type),
          field_name_(field_name), record_type_(record_type) {}

    [[nodiscard]] const std::string& raw()         const noexcept { return raw_; }
    [[nodiscard]] const std::string& target_type() const noexcept { return target_type_; }
    [[nodiscard]] const std::string& field_name()  const noexcept { return field_name_; }
    [[nodiscard]] const std::string& record_type() const noexcept { return record_type_; }

private:
    static std::string make_message(std::string_view raw,
                                    std::string_view target_type,
                                    std::string_view field_name,
                                    std::string_view record_type) {
        std::string m;
        m.reserve(64 + raw.size() + target_type.size() + field_name.size() + record_type.size());
        m.append("cannot convert '").append(raw)
         .append("' to ").append(target_type)
         .append(" (field='").append(field_name)
         .append("', record='").append(record_type).append("')");
        return m;
    }

    std::string raw_;
    std::string target_type_;
    std::string field_name_;
    std::string record_type_;
};

/// Malformed input encountered while tokenizing a record.
class MalformedRecordError : public ParseError {
public:
    using ParseError::ParseError;
};

/// I/O failure while reading or writing files.
class IoError : public ParseError {
public:
    using ParseError::ParseError;
};

/// Log content failed structural integrity checks (e.g. missing @BATCH).
class IntegrityError : public ParseError {
public:
    using ParseError::ParseError;
};

} // namespace ate
