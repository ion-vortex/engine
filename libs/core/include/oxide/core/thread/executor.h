#pragma once

#include <functional>

#include <oxide/core/export.h>

namespace oxide::core {

using Task = std::function<void()>;

class OXIDE_CORE_API IThreadPool {
public:
    virtual ~IThreadPool() = default;
    
    virtual void submit(Task&& task) = 0;
    virtual void waitIdle() = 0;
};

class OXIDE_CORE_API IExecutor {
public:
    virtual ~IExecutor() = default;
    
    virtual void execute(Task&& task) = 0;
};

} // namespace oxide::core
