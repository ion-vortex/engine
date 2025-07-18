#pragma once

#include <ion/core/export.h>
#include <ion/core/store.h>
#include <toml++/toml.hpp>
#include <mutex>
#include <fstream>

namespace ion::core::detail {

class TomlTransaction;

class ION_CORE_API TomlStore final : public IStore {
public:
    TomlStore(std::filesystem::path const& path, TomlStoreOptions const& options);
    ~TomlStore();

    std::expected<void, Error> open(std::filesystem::path const& path) override;
    std::expected<void, Error> close() override;
    std::expected<std::unique_ptr<ITransaction>, Error> begin_transaction() override;

private:
    friend class TomlTransaction;

    std::filesystem::path path_;
    TomlStoreOptions options_;
    toml::table data_;
    bool is_open_ = false;
    mutable std::mutex mutex_;
    
    std::expected<void, Error> load_from_file();
    std::expected<void, Error> save_to_file(toml::table const& data);
    void update_data(toml::table const& new_data) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = new_data;
    }
};

}  // namespace ion::core::detail
