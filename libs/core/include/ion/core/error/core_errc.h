#pragma once
#include <string>
#include <string_view>
#include <cstdint>
#include <exception>
#include <ion/core/export.h>

namespace ion::core {

enum class ION_CORE_API core_errc
{
    invalid_handle   = 1, // raw==0 or generation mismatch
    path_syntax,          // malformed "[", non-digit index, etc.
    key_not_found,        // child key missing
    index_out_of_range,   // array index >= size
    type_mismatch,        // wrong type for op
    io_failure,           // serialization / filesystem error
    parse_error,          // Storage system parse error,
    already_exists,       // key already exists in parent table
    invalid_state,        // operation not valid in current state
    message_too_long,     // message exceeds max length,
    invalid_argument,     // invalid argument passed to function
    unknown,              // unknown error
};

// core error category for std::error_code
class ION_CORE_API core_category final : public std::error_category {
public:
    const char* name() const noexcept override { return "ion.core"; }
    std::string message(int condition) const override 
    {
        using E = core_errc;
        switch (static_cast<E>(condition))
        {
            case E::invalid_handle:      return "Invalid handle";
            case E::path_syntax:         return "Path syntax error";
            case E::key_not_found:       return "Key not found";
            case E::index_out_of_range:  return "Index out of range";
            case E::type_mismatch:       return "Type mismatch";
            case E::io_failure:          return "I/O failure";
            case E::parse_error:         return "Parse error";
            case E::already_exists:      return "Already exists";
            case E::invalid_state:       return "Invalid state";
            case E::message_too_long:    return "Message too long";
            case E::invalid_argument:    return "Invalid argument";
            case E::unknown:             return "Unknown error";
            default:                     return "Unrecognised error";
        }
    }
};

extern ION_CORE_API const core_category k_core_category;

inline std::error_code make_error_code(core_errc e) noexcept {
    return {static_cast<int>(e), k_core_category};
}

} // namespace ion::core

template<> struct std::is_error_code_enum<ion::core::core_errc> : std::true_type {};