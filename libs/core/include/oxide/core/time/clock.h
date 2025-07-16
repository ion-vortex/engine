#pragma once

#include <cstdint>

namespace oxide::core {

class ITimer {
public:
    virtual ~ITimer() = default;
    
    [[nodiscard]]
    virtual uint64_t now_ns() const = 0;
};

} // namespace oxide::core
