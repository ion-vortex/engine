#include "vector_buffer_impl.h"

using namespace ion::core::detail;
using namespace ion::core;

std::expected<void, std::error_code>
vector_buffer::resize(std::size_t bytes) {
    if (bytes > data_.max_size()) {
        return std::unexpected(make_error_code(core_errc::message_too_long));
    }
    data_.resize(bytes);
    return {};
}

std::expected<void, std::error_code>
vector_buffer::reserve(std::size_t bytes) {
    if (bytes > data_.max_size()) {
        return std::unexpected(make_error_code(core_errc::message_too_long));
    }
    data_.reserve(bytes);
    return {};
}

std::expected<void, std::error_code>
vector_buffer::clear() {
    data_.clear();
    return {};
}

std::expected<void, std::error_code>
vector_buffer::shrink_to_fit() {
    data_.shrink_to_fit();
    return {};
}

std::expected<void, std::error_code>
vector_buffer::append(std::span<const std::byte> src) {
    if (data_.size() + src.size() > data_.max_size()) {
        return std::unexpected(make_error_code(core_errc::message_too_long));
    }
    data_.insert(data_.end(), src.begin(), src.end());
    return {};
}
std::span<const std::byte>
vector_buffer::view() const noexcept {
    return std::span<const std::byte>(data_.data(), data_.size());
}

std::span<std::byte>
vector_buffer::mutate() noexcept {
    return std::span<std::byte>(data_.data(), data_.size());
}

std::size_t
vector_buffer::size() const noexcept {
    return data_.size();
}

std::size_t
vector_buffer::capacity() const noexcept {
    return data_.capacity();
}