#pragma once

#include <oxide/core/export.h>

#include "i_buffer.h"
#include "static_buffer.h"

namespace oxide::core {

// Factory function to create a dynamic buffer with an optional initial capacity
[[nodiscard("Handle buffer creation result"), gnu::warn_unused_result]]
OXIDE_CORE_API std::expected<std::unique_ptr<IBuffer>, Error>
create_buffer(std::size_t initial_capacity = 0);

// Factory function to create a static buffer with a fixed size
template <std::size_t N>
[[nodiscard("Handle static buffer creation result"), gnu::warn_unused_result]]
std::expected<std::unique_ptr<IBuffer>, Error>
create_static_buffer() {
    return std::make_unique<detail::StaticBuffer<N>>();
}

} // namespace oxide::core