#pragma once

#include <string_view>
#include <cstdint>

#include <ion/core/export.h>

namespace ion::core {

class ION_CORE_API IMetrics {
public:
    virtual ~IMetrics() = default;
    
    virtual void increment(std::string_view name, uint64_t value = 1) = 0;
    virtual void gauge(std::string_view name, double value) = 0;
    virtual void timing(std::string_view name, uint64_t nanoseconds) = 0;
};

} // namespace ion::core
