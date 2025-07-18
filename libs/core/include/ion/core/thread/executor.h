#pragma once

#include <functional>

#include <ion/core/export.h>

namespace ion::core {

using Task = std::function<void()>;

class ION_CORE_API IThreadPool {
public:
    virtual ~IThreadPool() = default;
    
    virtual void submit(Task&& task) = 0;
    virtual void waitIdle() = 0;
};

class ION_CORE_API IExecutor {
public:
    virtual ~IExecutor() = default;
    
    virtual void execute(Task&& task) = 0;
};

} // namespace ion::core
