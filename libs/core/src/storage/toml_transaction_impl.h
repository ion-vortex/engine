#pragma once

#include <oxide/core/store/i_transaction.h>
#include <oxide/core/error.h>
#include <oxide/core/store/store_handle.h>
#include <expected>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <toml++/toml.h>

namespace oxide::core {

/**
 * @brief Implementation of ITransaction for TOML-based storage.
 *
 * This class provides a concrete implementation of the ITransaction interface,
 * using an in-memory representation of TOML data. It supports ACID-compliant
 * operations on hierarchical storage data.
 */
class TomlTransaction final : public ITransaction {
public:
    TomlTransaction();
    ~TomlTransaction() noexcept override;

    [[nodiscard]] std::expected<StoreHandle, Error> root() const override;
    [[nodiscard]] std::expected<bool, Error> get_bool(StoreHandle h) const override;
    [[nodiscard]] std::expected<int64_t, Error> get_int(StoreHandle h) const override;
    [[nodiscard]] std::expected<double, Error> get_double(StoreHandle h) const override;
    [[nodiscard]] std::expected<std::string, Error> get_string(StoreHandle h) const override;

    [[nodiscard]] std::expected<void, Error> set_bool(StoreHandle h, bool v) override;
    [[nodiscard]] std::expected<void, Error> set_int(StoreHandle h, int64_t v) override;
    [[nodiscard]] std::expected<void, Error> set_double(StoreHandle h, double v) override;
    [[nodiscard]] std::expected<void, Error> set_string(StoreHandle h, std::string_view v) override;

    [[nodiscard]] std::expected<StoreHandle, Error> make_array(StoreHandle parent, std::string_view key) override;
    [[nodiscard]] std::expected<StoreHandle, Error> make_object(StoreHandle parent, std::string_view key) override;
    [[nodiscard]] std::expected<void, Error> make_bool(StoreHandle parent, std::string_view key, bool v) override;
    [[nodiscard]] std::expected<void, Error> make_int(StoreHandle parent, std::string_view key, int64_t v) override;
    [[nodiscard]] std::expected<void, Error> make_double(StoreHandle parent, std::string_view key, double v) override;
    [[nodiscard]] std::expected<void, Error> make_string(StoreHandle parent, std::string_view key, std::string_view v) override;

    [[nodiscard]] std::expected<void, Error> remove(StoreHandle parent, std::string_view key) override;
    [[nodiscard]] std::expected<bool, Error> has(StoreHandle parent, std::string_view key) const override;
    [[nodiscard]] std::expected<void, Error> erase_element(StoreHandle parent, size_t idx) override;
    [[nodiscard]] std::expected<bool, Error> has_element(StoreHandle parent, size_t idx) const override;
    [[nodiscard]] std::expected<StoreHandle, Error> child(StoreHandle parent, std::string_view key) const override;
    [[nodiscard]] std::expected<StoreHandle, Error> element(StoreHandle parent, size_t idx) const override;

private:
    [[nodiscard]] std::expected<void, Error> commit_impl() override;
    void rollback_impl() noexcept override;

    // Internal representation of TOML data using toml::table.
    toml::table data_;
    bool committed_ = false;
};

} // namespace oxide::core
