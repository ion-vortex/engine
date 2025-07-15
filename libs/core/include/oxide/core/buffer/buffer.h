#pragma once
#include <span>
#include <memory>
#include <expected>

namespace oxide::core {

class IBuffer {
public:
    virtual ~IBuffer() = default;

    // Grow (realloc) or shrink in-place; may return unexpected<Error>.
    [[nodiscard]] virtual std::expected<void, Error> resize(std::size_t bytes) = 0;

    // Append raw bytes.
    [[nodiscard]] virtual std::expected<void, Error> append(std::span<const std::byte> src) = 0;

    // Read/write accessors.
    [[nodiscard]] virtual std::span<const std::byte> view() const noexcept = 0;
    [[nodiscard]] virtual std::span<      std::byte> mutate() noexcept      = 0;

    [[nodiscard]] virtual std::size_t size() const noexcept = 0;
};

// Factory
[[nodiscard]] std::expected<std::unique_ptr<IBuffer>, Error>
makeDefaultBuffer(std::size_t initial = 0);

} // namespace oxide::core