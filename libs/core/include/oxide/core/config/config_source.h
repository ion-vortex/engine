#pragma once

#include <string>
#include <string_view>
#include <expected>

namespace oxide::core {

enum class ConfigError {
    KeyNotFound,
    ParseError,
    AccessDenied
};

class IConfigSource {
public:
    virtual ~IConfigSource() = default;
    
    [[nodiscard]]
    virtual std::expected<std::string, ConfigError> get(std::string_view key) const = 0;
    
    [[nodiscard]]
    virtual bool contains(std::string_view key) const = 0;
};

} // namespace oxide::core
