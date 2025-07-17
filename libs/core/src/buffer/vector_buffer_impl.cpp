#include "vector_buffer_impl.h"

using namespace oxide::core::detail;
using namespace oxide::core;

std::expected<void, Error>
VectorBuffer::resize(std::size_t bytes) {
    if (bytes > data_.max_size()) {
        return std::unexpected(Error(ErrorCode::MessageTooLong, "Resize exceeds vector buffer size"));
    }
    data_.resize(bytes);
    return {};
}

std::expected<void, Error>
VectorBuffer::reserve(std::size_t bytes) {
    if (bytes > data_.max_size()) {
        return std::unexpected(Error(ErrorCode::MessageTooLong, "Reserve exceeds vector buffer size"));
    }
    data_.reserve(bytes);
    return {};
}

std::expected<void, Error>
VectorBuffer::clear() {
    data_.clear();
    return {};
}

std::expected<void, Error>
VectorBuffer::shrink_to_fit() {
    data_.shrink_to_fit();
    return {};
}

std::expected<void, Error>
VectorBuffer::append(std::span<const std::byte> src) {
    if (data_.size() + src.size() > data_.max_size()) {
        return std::unexpected(Error(ErrorCode::MessageTooLong, "Append exceeds vector buffer size"));
    }
    data_.insert(data_.end(), src.begin(), src.end());
    return {};
}
std::span<const std::byte>
VectorBuffer::view() const noexcept {
    return std::span<const std::byte>(data_.data(), data_.size());
}

std::span<std::byte>
VectorBuffer::mutate() noexcept {
    return std::span<std::byte>(data_.data(), data_.size());
}

std::size_t
VectorBuffer::size() const noexcept {
    return data_.size();
}

std::size_t
VectorBuffer::capacity() const noexcept {
    return data_.capacity();
}