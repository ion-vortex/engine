#include <ion/core/buffer.h>
#include "vector_buffer_impl.h"

namespace ion::core
{

std::expected<std::unique_ptr<IBuffer>, Error>
create_buffer(std::size_t initial_capacity)
{
    auto buf = std::make_unique<detail::VectorBuffer>();
    if (initial_capacity) {
        if (auto e = buf->reserve(initial_capacity); !e.has_value()) {
            return std::unexpected(e.error());
        }
    }
    return buf;
}

} // namespace ion::core
