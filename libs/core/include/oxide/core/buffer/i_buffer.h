#pragma once
#include <span>
#include <memory>
#include <expected>
#include <oxide/core/error.h>
#include <oxide/core/export.h>

namespace oxide::core {

class OXIDE_CORE_API IBuffer {
public:
    virtual ~IBuffer() = default;

    [[nodiscard("Handle resize result"), gnu::warn_unused_result]]
    virtual std::expected<void, Error> resize(std::size_t bytes) = 0;

    [[nodiscard("Handle reserve result"), gnu::warn_unused_result]]
    virtual std::expected<void, Error> reserve(std::size_t bytes) = 0;

    [[nodiscard("Handle clear result"), gnu::warn_unused_result]]
    virtual std::expected<void, Error> clear() = 0;

    [[nodiscard("Handle shrink result"), gnu::warn_unused_result]]
    virtual std::expected<void, Error> shrink_to_fit() = 0;

    [[nodiscard("Handle append result"), gnu::warn_unused_result]]
    virtual std::expected<void, Error> append(std::span<const std::byte> src) = 0;

    [[nodiscard("Handle view result"), gnu::warn_unused_result]]
    virtual std::span<const std::byte> view() const noexcept = 0;

    [[nodiscard("Handle mutate result"), gnu::warn_unused_result]]
    virtual std::span<      std::byte> mutate() noexcept      = 0;

    [[nodiscard("Handle size result"), gnu::warn_unused_result]]
    virtual std::size_t size() const noexcept = 0;

    [[nodiscard("Handle capacity result"), gnu::warn_unused_result]]
    virtual std::size_t capacity() const noexcept = 0;
};

} // namespace oxide::core