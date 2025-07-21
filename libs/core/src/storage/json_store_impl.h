#pragma once

#include <ion/core/export.h>
#include <ion/core/store.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <fstream>

namespace ion::core::detail {

class json_transaction;

class ION_CORE_API json_store final : public store_base {
public:
    json_store(std::filesystem::path const& path, json_store_options const& options);
    ~json_store();

    std::expected<void, std::error_code> open(std::filesystem::path const& path) override;
    std::expected<void, std::error_code> close() override;
    std::expected<std::unique_ptr<ITransaction>, std::error_code> begin_transaction() override;

private:
    friend class json_transaction;

    std::filesystem::path path_;
    json_store_options options_;
    nlohmann::json data_;
    bool is_open_ = false;
    mutable std::mutex mutex_;
    
    std::expected<void, std::error_code> load_from_file();
    std::expected<void, std::error_code> save_to_file(nlohmann::json const& data);
    void update_data(nlohmann::json const& new_data) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = new_data;
    }
};

}  // namespace ion::core::detail