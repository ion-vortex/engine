#include <oxide/core/export.h>
#include <oxide/core/store.h>

#include "toml_store_impl.h"
#include "toml_transaction_impl.h"

namespace oxide::core {

std::expected<std::unique_ptr<IStore>, Error>
make_toml_file_store(std::filesystem::path const& path, TomlStoreOptions opts) {
    auto store = std::make_unique<detail::TomlStore>(path, opts);
    return store;
}

}  // namespace oxide::core
