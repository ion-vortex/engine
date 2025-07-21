#pragma once

#include <functional>

#include <ion/core/export.h>

namespace ion::core {

using Task = std::function<void()>;

class ION_CORE_API thread_pool_base {
public:
    virtual ~thread_pool_base() = default;
    
    virtual void submit(Task&& task) = 0;
    virtual void wait_idle() = 0;
};

class ION_CORE_API executor_base {
public:
    virtual ~executor_base() = default;
    
    virtual void execute(Task&& task) = 0;
};

} // namespace ion::core
