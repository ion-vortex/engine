#pragma once

namespace oxide::core {

class IService {
public:
    virtual ~IService() = default;
    
    virtual void init() = 0;
    virtual void tick() = 0;
    virtual void shutdown() = 0;
};

} // namespace oxide::core
