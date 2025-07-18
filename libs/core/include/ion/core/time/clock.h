#pragma once

#include <cstdint>

#include <ion/core/export.h>

namespace ion::core {

class ION_CORE_API IClock {
public:
    virtual ~IClock() = default;
    
    [[nodiscard, gnu::warn_unused_result]]

    virtual uint64_t now_ns() const = 0;
};

} // namespace ion::core
