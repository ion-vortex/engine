#pragma once

#include <ion/core/store/i_transaction.h>
#include <ion/core/error.h>
#include <ion/core/store/store_handle.h>
#include <expected>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <toml++/toml.h>

namespace ion::core::detail {

class TomlStore;

/**
 * @brief Implementation of ITransaction for TOML-based storage.
 *
 * This class provides a concrete implementation of the ITransaction interface,
 * using an in-memory representation of TOML data. It supports ACID-compliant
 * operations on hierarchical storage data.
 */
class ION_CORE_API TomlTransaction final : public ITransaction {
public:
    TomlTransaction(toml::table const& initial_data, TomlStore* store, TomlStoreOptions const& options);
    ~TomlTransaction() noexcept override;

    [[nodiscard, gnu::warn_unused_result]]
 std::expected<StoreHandle, Error> root() const override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<bool, Error> get_bool(StoreHandle h) const override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<int64_t, Error> get_int(StoreHandle h) const override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<double, Error> get_double(StoreHandle h) const override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<std::string, Error> get_string(StoreHandle h) const override;

    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> set_bool(StoreHandle h, bool v) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> set_int(StoreHandle h, int64_t v) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> set_double(StoreHandle h, double v) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> set_string(StoreHandle h, std::string_view v) override;

    [[nodiscard, gnu::warn_unused_result]]
 std::expected<StoreHandle, Error> make_array(StoreHandle parent, std::string_view key) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<StoreHandle, Error> make_object(StoreHandle parent, std::string_view key) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> make_bool(StoreHandle parent, std::string_view key, bool v) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> make_int(StoreHandle parent, std::string_view key, int64_t v) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> make_double(StoreHandle parent, std::string_view key, double v) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> make_string(StoreHandle parent, std::string_view key, std::string_view v) override;

    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> remove(StoreHandle parent, std::string_view key) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<bool, Error> has(StoreHandle parent, std::string_view key) const override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> erase_element(StoreHandle parent, size_t idx) override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<bool, Error> has_element(StoreHandle parent, size_t idx) const override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<StoreHandle, Error> child(StoreHandle parent, std::string_view key) const override;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<StoreHandle, Error> element(StoreHandle parent, size_t idx) const override;

private:
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<void, Error> commit_impl() override;
    void rollback_impl() noexcept override;

    // Node representation for handle mapping
    struct Node {
        std::vector<std::string> path;  // Path from root to reconstruct node access
    };

    toml::table data_;
    TomlStore* store_;
    TomlStoreOptions options_;
    mutable std::unordered_map<uint64_t, Node> handle_map_;
    mutable uint64_t next_handle_ = 1;
    
    [[nodiscard, gnu::warn_unused_result]]
 StoreHandle make_handle(std::vector<std::string> path) const;
    [[nodiscard, gnu::warn_unused_result]]
 toml::node* get_node(StoreHandle h);
    [[nodiscard, gnu::warn_unused_result]]
 toml::node const* get_node(StoreHandle h) const;
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<toml::node*, Error> get_node_checked(StoreHandle h);
    [[nodiscard, gnu::warn_unused_result]]
 std::expected<toml::node const*, Error> get_node_checked(StoreHandle h) const;
    [[nodiscard, gnu::warn_unused_result]]
 toml::node* navigate_to_node(std::vector<std::string> const& path);
    [[nodiscard, gnu::warn_unused_result]]
 toml::node const* navigate_to_node(std::vector<std::string> const& path) const;
    [[nodiscard, gnu::warn_unused_result]]
 bool is_valid_key(std::string_view key) const;
};

} // namespace ion::core
