#pragma once
#include <span>
#include <memory>
#include <expected>
#include <ion/core/error.h>
#include <ion/core/export.h>

namespace ion::core {

class ION_CORE_API buffer_base {
public:
    virtual ~buffer_base() = default;

    [[ION_NODISCARD("Handle resize result")]]
    virtual std::expected<void, std::error_code> resize(std::size_t bytes) = 0;

    [[ION_NODISCARD("Handle reserve result")]]
    virtual std::expected<void, std::error_code> reserve(std::size_t bytes) = 0;

    [[ION_NODISCARD("Handle clear result")]]
    virtual std::expected<void, std::error_code> clear() = 0;

    [[ION_NODISCARD("Handle shrink result")]]
    virtual std::expected<void, std::error_code> shrink_to_fit() = 0;

    [[ION_NODISCARD("Handle append result")]]
    virtual std::expected<void, std::error_code> append(std::span<const std::byte> src) = 0;

    [[ION_NODISCARD("Handle view result")]]
    virtual std::span<const std::byte> view() const noexcept = 0;

    [[ION_NODISCARD("Handle mutate result")]]
    virtual std::span<      std::byte> mutate() noexcept      = 0;

    [[ION_NODISCARD("Handle size result")]]
    virtual std::size_t size() const noexcept = 0;

    [[ION_NODISCARD("Handle capacity result")]]
    virtual std::size_t capacity() const noexcept = 0;
};

} // namespace ion::core