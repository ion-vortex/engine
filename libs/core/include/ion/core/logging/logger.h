#pragma once

#include <string_view>
#include <format>

#include <ion/core/export.h>

namespace ion::core {

enum class ION_CORE_API log_level {
    trace,   // super-verbose
    debug,   // developer diagnostics
    info,    // normal ops
    warning, // something looks off
    error,   // recoverable failure
    critical // about to crash / abort
};

class ION_CORE_API logger_base {
public:
    virtual ~logger_base() = default;
    
    virtual void log(log_level level, std::string_view msg) = 0;

    template <typename... Args>
    inline void log(log_level level, std::string_view fmt, Args&&... args) {
        log(level,
            std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)));
    }
    
    [[ION_NODISCARD("You should be actually checking this if you're bothering to call it.")]]
    virtual bool is_enabled(log_level level) const = 0;
};

} // namespace ion::core
