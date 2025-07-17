#pragma once

#include <oxide/core/export.h>
#include <oxide/core/store.h>

#include <toml++/toml.hpp>

namespace oxide::core::detail {

class TomlStore final : public IStore {
public:
    TomlStore(std::filesystem::path const& path, TomlStoreOptions const& options);
    ~TomlStore();

    std::expected<void, Error> open(std::filesystem::path const& path) override;
    std::expected<void, Error> close() override;
    std::expected<std::unique_ptr<ITransaction>, Error> begin_transaction() override;

private:
    std::filesystem::path path_;
    TomlStoreOptions options_;
    toml::table data_;
};

}  // namespace oxide::core::detail
