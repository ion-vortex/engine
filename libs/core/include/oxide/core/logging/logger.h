#pragma once

#include <string_view>
#include <format>

#include <oxide/core/export.h>

namespace oxide::core {

enum class OXIDE_CORE_API LogLevel {
    Trace,   // super-verbose
    Debug,   // developer diagnostics
    Info,    // normal ops
    Warning, // something looks off
    Error,   // recoverable failure
    Critical // about to crash / abort
};

class OXIDE_CORE_API ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void log(LogLevel level, std::string_view msg) = 0;

    template <typename... Args>
    inline void log(LogLevel level, std::string_view fmt, Args&&... args) {
        log(level,
            std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)));
    }
    
    [[nodiscard, gnu::warn_unused_result]]

    virtual bool isEnabled(LogLevel level) const = 0;
};

} // namespace oxide::core
