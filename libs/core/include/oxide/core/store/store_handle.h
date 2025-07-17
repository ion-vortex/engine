#pragma once

#include <oxide/core/export.h>
#include <cstdint>

namespace oxide::core {

/**
 * @brief Opaque handle to a node in a storage tree.
 *
 * StoreHandle is used to reference objects, arrays, or values within a storage backend.
 * A value of 0 is always invalid.
 */
struct OXIDE_CORE_API StoreHandle {
    /**
     * @brief Raw handle value. 0 is always invalid.
     */
    uint64_t raw = 0;

    /**
     * @brief Returns true if the handle is valid (nonzero).
     */
    [[nodiscard("Check if handle is valid")]]
    constexpr bool valid() const noexcept { return raw != 0; }

    /**
     * @brief Compares two handles for equality.
     * @param o The other handle.
     * @return True if both refer to the same node.
     */
    [[nodiscard("Check if handles are equal")]]
    bool operator==(StoreHandle o) const noexcept { return raw == o.raw; }
};

}  // namespace oxide::core
