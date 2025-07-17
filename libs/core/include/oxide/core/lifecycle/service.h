#pragma once

#include <oxide/core/export.h>

namespace oxide::core {

class OXIDE_CORE_API IService {
public:
    virtual ~IService() = default;
    
    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void shutdown() = 0;
};

} // namespace oxide::core
