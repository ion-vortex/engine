#pragma once

#include <ion/core/export.h>
#include <ion/core/store.h>
#include <toml++/toml.hpp>
#include <mutex>
#include <fstream>

namespace ion::core::detail {

class toml_transaction;

class ION_CORE_API toml_store final : public store_base {
public:
    toml_store(std::filesystem::path const& path, toml_store_options const& options);
    ~toml_store();

    std::expected<void, std::error_code> open(std::filesystem::path const& path) override;
    std::expected<void, std::error_code> close() override;
    std::expected<std::unique_ptr<ITransaction>, std::error_code> begin_transaction() override;

private:
    friend class toml_transaction;

    std::filesystem::path path_;
    toml_store_options options_;
    toml::table data_;
    bool is_open_ = false;
    mutable std::mutex mutex_;
    
    std::expected<void, std::error_code> load_from_file();
    std::expected<void, std::error_code> save_to_file(toml::table const& data);
    void update_data(toml::table const& new_data) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = new_data;
    }
};

}  // namespace ion::core::detail
