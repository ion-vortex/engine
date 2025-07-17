#pragma once

#include <oxide/core/export.h>
#include <oxide/core/error.h>
#include <expected>
#include <string_view>
#include <system_error>
#include <charconv>

#include "store_handle.h"


/**
 * @brief Abstract interface for ACID-compliant storage transactions.
 *
 * This interface defines the contract for transactional operations on storage objects,
 * supporting atomicity, consistency, isolation, and durability (ACID) semantics.
 * Implementations must ensure that changes are either fully committed or rolled back,
 * and provide methods for manipulating and querying hierarchical storage data.
 *
 * @note Thread-safety: neither IStore nor ITransaction are thread-safe. Concurrent access must be synchronized externally.
 *
 * @note Path rules: Keys in path-strings must match `[A-Za-z_][A-Za-z0-9_]*`. No quoting/escaping is supported; invalid segments yield PathSyntax.
 */
namespace oxide::core {

class OXIDE_CORE_API ITransaction {
public:
    /**
     * @brief Destructor. Rolls back if not committed.
     */
    virtual ~ITransaction() noexcept {
        if (!committed_) rollback_impl();
    }

    /**
     * @brief Rolls back the transaction if not already committed.
     *
     * After rollback, the transaction is considered committed (finalized).
     */
    void rollback() noexcept {
        if (!committed_) {
            rollback_impl();
            committed_ = true;
        }
    }

    /**
     * @brief Returns the root handle of the storage tree.
     * @return StoreHandle on success, or an error (e.g. IoFailure) on failure.
     */
    [[nodiscard("Check for error or valid root handle")]]
    virtual std::expected<StoreHandle, Error> root() const = 0;

    /**
     * @brief Retrieves a boolean value from the given handle.
     * @param h The handle to query.
     * @return The boolean value or an error.
     */
    [[nodiscard("Check for error or valid bool value")]]
    virtual std::expected<bool,Error>
    get_bool   (StoreHandle h) const = 0;

    /**
     * @brief Retrieves an integer value from the given handle.
     * @param h The handle to query.
     * @return The integer value or an error.
     */
    [[nodiscard("Check for error or valid int value")]]
    virtual std::expected<int64_t,Error>
    get_int    (StoreHandle h) const = 0;

    /**
     * @brief Retrieves a double value from the given handle.
     * @param h The handle to query.
     * @return The double value or an error.
     */
    [[nodiscard("Check for error or valid double value")]]
    virtual std::expected<double,Error>
    get_double (StoreHandle h) const = 0;

    /**
     * @brief Retrieves a string value from the given handle.
     * @param h The handle to query.
     * @return The string value or an error.
     */
    [[nodiscard("Check for error or valid string value")]]
    virtual std::expected<std::string,Error>
    get_string (StoreHandle h) const = 0;

    /**
     * @brief Sets a boolean value at the given handle.
     * @param h The handle to modify.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on set_bool")]]
    virtual std::expected<void,Error>
    set_bool   (StoreHandle h, bool   v) = 0;

    /**
     * @brief Sets an integer value at the given handle.
     * @param h The handle to modify.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on set_int")]]
    virtual std::expected<void,Error>
    set_int    (StoreHandle h, int64_t v) = 0;

    /**
     * @brief Sets a double value at the given handle.
     * @param h The handle to modify.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on set_double")]]
    virtual std::expected<void,Error>
    set_double (StoreHandle h, double  v) = 0;

    /**
     * @brief Sets a string value at the given handle.
     * @param h The handle to modify.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on set_string")]]
    virtual std::expected<void,Error>
    set_string (StoreHandle h, std::string_view v) = 0;

    /**
     * @brief Creates an array as a child of the given parent and key.
     * @param parent The parent handle.
     * @param key The key for the new array.
     * @return Handle to the new array or error.
     */
    [[nodiscard("Check for error or valid array handle")]]
    virtual std::expected<StoreHandle,Error>
    make_array  (StoreHandle parent, std::string_view key) = 0;

    /**
     * @brief Creates an object as a child of the given parent and key.
     * @param parent The parent handle.
     * @param key The key for the new object.
     * @return Handle to the new object or error.
     */
    [[nodiscard("Check for error or valid object handle")]]
    virtual std::expected<StoreHandle,Error>
    make_object (StoreHandle parent, std::string_view key) = 0;

    /**
     * @brief Creates a boolean value as a child of the given parent and key.
     * @param parent The parent handle.
     * @param key The key for the new value.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on make_bool")]]
    virtual std::expected<void,Error>
    make_bool   (StoreHandle parent, std::string_view key, bool   v) = 0;

    /**
     * @brief Creates an integer value as a child of the given parent and key.
     * @param parent The parent handle.
     * @param key The key for the new value.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on make_int")]]
    virtual std::expected<void,Error>
    make_int    (StoreHandle parent, std::string_view key, int64_t v) = 0;

    /**
     * @brief Creates a double value as a child of the given parent and key.
     * @param parent The parent handle.
     * @param key The key for the new value.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on make_double")]]
    virtual std::expected<void,Error>
    make_double (StoreHandle parent, std::string_view key, double  v) = 0;

    /**
     * @brief Creates a string value as a child of the given parent and key.
     * @param parent The parent handle.
     * @param key The key for the new value.
     * @param v The value to set.
     * @return Success or error.
     */
    [[nodiscard("Check for error on make_string")]]
    virtual std::expected<void,Error>
    make_string (StoreHandle parent, std::string_view key, std::string_view v) = 0;

    /**
     * @brief Removes a child by key from the given parent.
     * @param parent The parent handle.
     * @param key The key to remove.
     * @return Success or error.
     */
    [[nodiscard("Check for error on remove")]]
    virtual std::expected<void,Error>
    remove        (StoreHandle parent, std::string_view key) = 0;

    /**
     * @brief Checks if a child with the given key exists under the parent.
     * @param parent The parent handle.
     * @param key The key to check.
     * @return True if exists, false otherwise, or error.
     */
    [[nodiscard("Check for error or existence result")]]
    virtual std::expected<bool,Error>
    has           (StoreHandle parent, std::string_view key) const = 0;

    /**
     * @brief Removes an element by index from the given parent array.
     * @param parent The parent array handle.
     * @param idx The index to remove.
     * @return Success or error.
     */
    [[nodiscard("Check for error on erase_element")]]
    virtual std::expected<void,Error>
    erase_element (StoreHandle parent, size_t idx) = 0;

    /**
     * @brief Checks if an element exists at the given index in the parent array.
     * @param parent The parent array handle.
     * @param idx The index to check.
     * @return True if exists, false otherwise, or error.
     */
    [[nodiscard("Check for error or existence result")]]
    virtual std::expected<bool,Error>
    has_element   (StoreHandle parent, size_t idx) const = 0;

    /**
     * @brief Retrieves a child handle by key from the given parent.
     * @param parent The parent handle.
     * @param key The key to retrieve.
     * @return The child handle or error.
     */
    [[nodiscard("Check for error or valid child handle")]]
    virtual std::expected<StoreHandle,Error>
    child   (StoreHandle parent, std::string_view key) const = 0;

    /**
     * @brief Retrieves an element handle by index from the given parent array.
     * @param parent The parent array handle.
     * @param idx The index to retrieve.
     * @return The element handle or error.
     */
    [[nodiscard("Check for error or valid element handle")]]
    virtual std::expected<StoreHandle,Error>
    element (StoreHandle parent, size_t idx) const = 0;

    /**
     * @brief Commits the transaction, making all changes durable.
     * @return Success or error.
     */
    [[nodiscard("Check for error on commit")]]
    std::expected<void,Error> commit() {
        auto e = commit_impl();
        if (e) committed_ = true;
        return e;
    }

    /**
     * @brief Navigates from a base handle using a dot/bracket path.
     *
     * Supports dot notation for objects and bracket notation for arrays.
     * @param base The starting handle.
     * @param path The navigation path (e.g. "foo.bar[2].baz").
     * @return The resulting handle or error.
     */
    [[nodiscard("Check for error or valid navigation result")]]
    std::expected<StoreHandle,Error>
    navigate(StoreHandle base, std::string_view path) const {
        if (!base.valid()) return std::unexpected(ErrorCode::InvalidHandle);
        StoreHandle cur = base;
        size_t i = 0, n = path.size();
        while (i < n) {
            if (path[i] == '.') { ++i; continue; }
            if (path[i] == '[') {
                ++i; size_t start = i;
                while (i < n && path[i] >= '0' && path[i] <= '9') ++i;
                if (i >= n || path[i] != ']') return std::unexpected(ErrorCode::PathSyntax);
                uint64_t idx = 0;
                auto [ptr, ec] = std::from_chars(path.data()+start, path.data()+i, idx);
                if (ec == std::errc::invalid_argument)   return std::unexpected(ErrorCode::PathSyntax);
                if (ec == std::errc::result_out_of_range) return std::unexpected(ErrorCode::IndexOutOfRange);
                auto next = element(cur, idx);
                if (!next) return next;
                cur = *next;
                ++i;
            } else {
                size_t j = i;
                while (j < n && path[j] != '.' && path[j] != '[') ++j;
                auto key = path.substr(i, j - i);
                auto next = child(cur, key);
                if (!next) return next;
                cur = *next;
                i = j;
            }
            if (!cur.valid()) return std::unexpected(ErrorCode::KeyNotFound);
        }
        return cur;
    }

    /**
     * @brief Retrieves a value of type T from a path under a base handle.
     *
     * Supported types: bool, int64_t, double, std::string.
     * @tparam T The value type to retrieve.
     * @param base The starting handle.
     * @param path The navigation path.
     * @return The value or error.
     */
    template<typename T>
    [[nodiscard("Check for error or valid value")]]
    std::expected<T,Error>
    get(StoreHandle base, std::string_view path) const {
        auto h = navigate(base, path);
        if (!h) return std::unexpected(h.error());
        if constexpr (std::is_same_v<T,bool>)       return get_bool(*h);
        else if constexpr (std::is_same_v<T,int64_t>) return get_int(*h);
        else if constexpr (std::is_same_v<T,double>)  return get_double(*h);
        else if constexpr (std::is_same_v<T,std::string>) return get_string(*h);
        else static_assert(sizeof(T)==0, "Unsupported get<> type");
    }

private:
    /**
     * @brief Implementation of commit. Must be provided by concrete class.
     */
    [[nodiscard("Check for error on commit_impl")]]
    virtual std::expected<void,Error> commit_impl() = 0;
    /**
     * @brief Implementation of rollback. Must be provided by concrete class.
     */
    virtual void                   rollback_impl() noexcept = 0;
    /**
     * @brief True if the transaction has been committed or rolled back.
     */
    bool committed_ = false;
};
}