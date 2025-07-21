#pragma once

#include <ion/core/export.h>
#include <cstdint>

namespace ion::core {

/**
 * @brief Opaque handle to a node in a storage tree.
 *
 * store_handle is used to reference objects, arrays, or values within a storage backend.
 * A value of 0 is always invalid.
 */
struct ION_CORE_API store_handle {
    /**
     * @brief Raw handle value. 0 is always invalid.
     */
    uint64_t raw = 0;

    /**
     * @brief Returns true if the handle is valid (nonzero).
     */
    [[OAT_NODISCARD("Check if handle is valid")]]
    constexpr bool valid() const noexcept { return raw != 0; }

    /**
     * @brief Compares two handles for equality.
     * @param o The other handle.
     * @return True if both refer to the same node.
     */
    [[OAT_NODISCARD("Check if handles are equal")]]
    bool operator==(store_handle o) const noexcept { return raw == o.raw; }
};

}  // namespace ion::core
