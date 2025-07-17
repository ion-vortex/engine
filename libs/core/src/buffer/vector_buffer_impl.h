#pragma once

#include <oxide/core/export.h>
#include <oxide/core/buffer.h>
#include <vector>

namespace oxide::core::detail {

class OXIDE_CORE_API VectorBuffer final : public IBuffer {
    std::vector<std::byte> data_;
public:
    [[nodiscard("Handle resize result")]]
    std::expected<void, Error> resize(std::size_t bytes) override;
    
    [[nodiscard("Handle reserve result")]]
    std::expected<void, Error> reserve(std::size_t bytes) override;
    
    [[nodiscard("Handle clear result")]]
    std::expected<void, Error> clear() override;
    
    [[nodiscard("Handle shrinkToFit result")]]
    std::expected<void, Error> shrinkToFit() override;
    
    [[nodiscard("Handle append result")]]
    std::expected<void, Error> append(std::span<const std::byte> src) override;

    std::span<const std::byte> view() const noexcept override;
    
    std::span<std::byte> mutate() noexcept override;
    
    std::size_t size() const noexcept override;
    
    std::size_t capacity() const noexcept override;
};

} // namespace oxide::core::detail