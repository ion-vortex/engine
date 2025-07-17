#pragma once

#include <cstdint>

#include <oxide/core/export.h>

namespace oxide::core {

class OXIDE_CORE_API IClock {
public:
    virtual ~IClock() = default;
    
    [[nodiscard]]
    virtual uint64_t now_ns() const = 0;
};

} // namespace oxide::core
