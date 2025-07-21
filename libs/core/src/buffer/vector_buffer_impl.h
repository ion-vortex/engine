#pragma once

#include <ion/core/export.h>
#include <ion/core/buffer.h>
#include <vector>

namespace ion::core::detail {

class ION_CORE_API vector_buffer final : public buffer_base {
    std::vector<std::byte> data_;
public:
    [[OAT_NODISCARD("Handle resize result")]]
    std::expected<void, std::error_code> resize(std::size_t bytes) override;
    
    [[OAT_NODISCARD("Handle reserve result")]]
    std::expected<void, std::error_code> reserve(std::size_t bytes) override;
    
    [[OAT_NODISCARD("Handle clear result")]]
    std::expected<void, std::error_code> clear() override;
    
    [[OAT_NODISCARD("Handle shrinkToFit result")]]
    std::expected<void, std::error_code> shrink_to_fit() override;
    
    [[OAT_NODISCARD("Handle append result")]]
    std::expected<void, std::error_code> append(std::span<const std::byte> src) override;

    std::span<const std::byte> view() const noexcept override;
    
    std::span<std::byte> mutate() noexcept override;
    
    std::size_t size() const noexcept override;
    
    std::size_t capacity() const noexcept override;
};

} // namespace ion::core::detail