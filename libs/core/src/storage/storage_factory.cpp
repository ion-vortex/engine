#include <ion/core/export.h>
#include <ion/core/store.h>

#include "toml_store_impl.h"
#include "toml_transaction_impl.h"
#include "json_store_impl.h"
#include "json_transaction_impl.h"

namespace ion::core {

std::expected<std::unique_ptr<store_base>, std::error_code>
make_toml_file_store(std::filesystem::path const& path, toml_store_options opts) {
    auto store = std::make_unique<detail::toml_store>(path, opts);
    return store;
}

std::expected<std::unique_ptr<store_base>, std::error_code>
make_json_file_store(std::filesystem::path const& path, json_store_options opts) {
    auto store = std::make_unique<detail::json_store>(path, opts);
    return store;
}


}  // namespace ion::core
