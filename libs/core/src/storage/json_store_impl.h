#pragma once

#include <oxide/core/export.h>
#include <oxide/core/store.h>
#include <nlohmann/json.hpp>
#include <mutex>
#include <fstream>

namespace oxide::core::detail {

class JsonTransaction;

class OXIDE_CORE_API JsonStore final : public IStore {
public:
    JsonStore(std::filesystem::path const& path, JsonStoreOptions const& options);
    ~JsonStore();

    std::expected<void, Error> open(std::filesystem::path const& path) override;
    std::expected<void, Error> close() override;
    std::expected<std::unique_ptr<ITransaction>, Error> begin_transaction() override;

private:
    friend class JsonTransaction;

    std::filesystem::path path_;
    JsonStoreOptions options_;
    nlohmann::json data_;
    bool is_open_ = false;
    mutable std::mutex mutex_;
    
    std::expected<void, Error> load_from_file();
    std::expected<void, Error> save_to_file(nlohmann::json const& data);
    void update_data(nlohmann::json const& new_data) {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = new_data;
    }
};

}  // namespace oxide::core::detail