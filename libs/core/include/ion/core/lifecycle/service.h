#pragma once

#include <ion/core/export.h>

namespace ion::core {

class ION_CORE_API service_base {
public:
    virtual ~service_base() = default;

    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void shutdown() = 0;
};

} // namespace ion::core
