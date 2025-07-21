#pragma once

#include <cstdint>

#include <ion/core/export.h>

namespace ion::core {

class ION_CORE_API clock_base {
public:
    virtual ~clock_base() = default;
    
    [[ION_NODISCARD("If you're using this, you should be checking the result")]]
    virtual uint64_t now_ns() const = 0;
};

} // namespace ion::core
