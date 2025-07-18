#pragma once

#include <ion/core/export.h>
#include <ion/core/buffer.h>
#include <vector>

namespace ion::core::detail {

class ION_CORE_API VectorBuffer final : public IBuffer {
    std::vector<std::byte> data_;
public:
    [[nodiscard("Handle resize result"), gnu::warn_unused_result]]
    std::expected<void, Error> resize(std::size_t bytes) override;
    
    [[nodiscard("Handle reserve result"), gnu::warn_unused_result]]
    std::expected<void, Error> reserve(std::size_t bytes) override;
    
    [[nodiscard("Handle clear result"), gnu::warn_unused_result]]
    std::expected<void, Error> clear() override;
    
    [[nodiscard("Handle shrinkToFit result"), gnu::warn_unused_result]]
    std::expected<void, Error> shrink_to_fit() override;
    
    [[nodiscard("Handle append result"), gnu::warn_unused_result]]
    std::expected<void, Error> append(std::span<const std::byte> src) override;

    std::span<const std::byte> view() const noexcept override;
    
    std::span<std::byte> mutate() noexcept override;
    
    std::size_t size() const noexcept override;
    
    std::size_t capacity() const noexcept override;
};

} // namespace ion::core::detail