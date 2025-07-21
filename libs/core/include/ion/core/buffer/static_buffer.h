#pragma once
#include "buffer_base.h"
#include <array>
#include <cstring>
#include <ion/core/export.h>

namespace ion::core::detail {

template <std::size_t N>
class StaticBuffer final : public buffer_base {
    std::array<std::byte, N> data_;
    std::size_t size_ = 0;
public:
    std::expected<void, std::error_code> resize(std::size_t bytes) override {
        if (bytes > N) {
            return std::unexpected(make_error_code(core_errc::message_too_long));
        }
        size_ = bytes;
        return {};
    }

    std::expected<void, std::error_code> reserve(std::size_t bytes) override {
        if (bytes > N) {
            return std::unexpected(make_error_code(core_errc::message_too_long));
        }
        return {};
    }

    std::expected<void, std::error_code> clear() override {
        size_ = 0;
        return {};
    }

    std::expected<void, std::error_code> shrink_to_fit() override {
        // No-op for static buffer
        return {};
    }

    std::expected<void, std::error_code> append(std::span<const std::byte> src) override {
        if (size_ + src.size() > N) {
            return std::unexpected(make_error_code(core_errc::message_too_long));
        }
        std::memcpy(data_.data() + size_, src.data(), src.size());
        size_ += src.size();
        return {};
    }

    std::span<const std::byte> view() const noexcept override {
        return std::span<const std::byte>(data_.data(), size_);
    }

    std::span<std::byte> mutate() noexcept override {
        return std::span<std::byte>(data_.data(), size_);
    }

    std::size_t size() const noexcept override {
        return size_;
    }

    std::size_t capacity() const noexcept override {
        return N;
    }
};

} // namespace ion::core::detail