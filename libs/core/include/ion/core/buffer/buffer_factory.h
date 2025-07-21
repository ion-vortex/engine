#pragma once

#include <ion/core/export.h>

#include "buffer_base.h"
#include "static_buffer.h"

namespace ion::core {

// Factory function to create a dynamic buffer with an optional initial capacity
[[ION_NODISCARD("Handle buffer creation result")]]
ION_CORE_API std::expected<std::unique_ptr<buffer_base>, std::error_code>
create_buffer(std::size_t initial_capacity = 0);

// Factory function to create a static buffer with a fixed size
template <std::size_t N>
[[ION_NODISCARD("Handle static buffer creation result")]]
std::expected<std::unique_ptr<buffer_base>, std::error_code>
create_static_buffer() {
    return std::make_unique<detail::StaticBuffer<N>>();
}

} // namespace ion::core