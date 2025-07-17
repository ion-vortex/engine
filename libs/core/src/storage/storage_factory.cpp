#include <oxide/core/export.h>
#include <oxide/core/store.h>

#include "toml_store_impl.h"
#include "toml_transaction_impl.h"
#include "json_store_impl.h"
#include "json_transaction_impl.h"

namespace oxide::core {

std::expected<std::unique_ptr<IStore>, Error>
make_toml_file_store(std::filesystem::path const& path, TomlStoreOptions opts) {
    auto store = std::make_unique<detail::TomlStore>(path, opts);
    return store;
}

std::expected<std::unique_ptr<IStore>, Error>
make_json_file_store(std::filesystem::path const& path, JsonStoreOptions opts) {
    auto store = std::make_unique<detail::JsonStore>(path, opts);
    return store;
}


}  // namespace oxide::core
