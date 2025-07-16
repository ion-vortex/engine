#pragma once
#include <span>
#include <memory>
#include <expected>
#include <oxide/core/error/error.h>

namespace oxide::core {

class IBuffer {
public:
    virtual ~IBuffer() = default;

    [[nodiscard("Handle resize result")]]
    virtual std::expected<void, Error> resize(std::size_t bytes) = 0;

    [[nodiscard("Handle append result")]]
    virtual std::expected<void, Error> append(std::span<const std::byte> src) = 0;

    [[nodiscard("Handle view result")]]
    virtual std::span<const std::byte> view() const noexcept = 0;
    [[nodiscard("Handle mutate result")]]
    virtual std::span<      std::byte> mutate() noexcept      = 0;
    [[nodiscard("Handle size result")]]
    virtual std::size_t size() const noexcept = 0;
    [[nodiscard("Handle capacity result")]]
    virtual std::size_t capacity() const noexcept = 0;
};

// Factory
[[nodiscard("Handle buffer creation result")]]
std::expected<std::unique_ptr<IBuffer>, Error>
createBuffer(std::size_t initial_capacity = 0);

} // namespace oxide::core