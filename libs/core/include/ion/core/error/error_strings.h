#pragma once

#include <string_view>

#include "core_errc.h"

namespace ion::core {

    std::string core_category::message(int ev) const
    {
        using E = core_errc;
        switch (static_cast<E>(ev))
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
            default:                     return "Unrecognised error";
        }
    }
}