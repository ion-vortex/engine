#pragma once
#include "ibuffer.h"
#include <array>
#include <cstring>

namespace oxide::core::detail {

template <std::size_t N>
class StaticBuffer final : public IBuffer {
    std::array<std::byte, N> data_;
    std::size_t size_ = 0;
public:
    std::expected<void, Error> resize(std::size_t bytes) override {
        if (bytes > N) {
            return std::unexpected(Error(ErrorCode::MessageTooLong, "Resize exceeds static buffer size"));
        }
        size_ = bytes;
        return {};
    }

    std::expected<void, Error> reserve(std::size_t bytes) override {
        if (bytes > N) {
            return std::unexpected(Error(ErrorCode::MessageTooLong, "Reserve exceeds static buffer size"));
        }
        return {};
    }

    std::expected<void, Error> clear() override {
        size_ = 0;
        return {};
    }

    std::expected<void, Error> shrinkToFit() override {
        // No-op for static buffer
        return {};
    }

    std::expected<void, Error> append(std::span<const std::byte> src) override {
        if (size_ + src.size() > N) {
            return std::unexpected(Error(ErrorCode::MessageTooLong, "Append exceeds static buffer size"));
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

} // namespace oxide::core::detail