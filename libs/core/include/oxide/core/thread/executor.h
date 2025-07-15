#pragma once

#include <functional>

namespace oxide::core {

using Task = std::function<void()>;

class IThreadPool {
public:
    virtual ~IThreadPool() = default;
    
    virtual void submit(Task&& task) = 0;
    virtual void waitIdle() = 0;
};

class IExecutor {
public:
    virtual ~IExecutor() = default;
    
    virtual void execute(Task&& task) = 0;
};

} // namespace oxide::core
