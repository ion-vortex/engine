#pragma once

#include <ion/core/export.h>
#include <ion/core/error.h>
#include <ion/core/store/store_handle.h>
#include <expected>
#include <filesystem>
#include <memory>


namespace ion::core {

/**
 * @brief Options for file-backed stores (generic).
 *
 * Controls memory-mapping and journaling for file-based storage backends.
 */
struct ION_CORE_API file_store_options {
    /**
     * @brief Use memory-mapped writes if true.
     */
    bool write_mmap  = false;
    /**
     * @brief Enable journaling for crash safety.
     */
    bool use_journal = true;
};


/**
 * @brief Options for JSON file stores.
 *
 * Controls memory-mapping, journaling, and comment support for JSON backends.
 */
struct ION_CORE_API json_store_options {
    bool write_mmap     = false;   ///< Use memory-mapped writes if true.
    bool use_journal    = true;    ///< Enable journaling for crash safety.
    bool allow_comments = false;   ///< Allow comments in JSON files.
};


/**
 * @brief Options for TOML file stores.
 *
 * Controls memory-mapping, journaling, key order, and type strictness for TOML backends.
 */
struct ION_CORE_API toml_store_options {
    bool write_mmap     = false;   ///< Use memory-mapped writes if true.
    bool use_journal    = true;    ///< Enable journaling for crash safety.
    bool preserve_order = false;   ///< Preserve key order in TOML files.
    bool strict_types   = true;    ///< Enforce strict TOML type rules.
};


/**
 * @brief Abstract interface for a transactional storage backend.
 *
 * @note Thread-safety: neither store_base nor transaction_base are thread-safe. Concurrent access must be synchronized externally.
 *
 * @note Path rules: Keys in path-strings must match `[A-Za-z_][A-Za-z0-9_]*`. No quoting/escaping is supported; invalid segments yield PathSyntax.
 *
 * Provides methods to open/close the store and begin transactions.
 */
class ION_CORE_API store_base {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~store_base() = default;

    /**
     * @brief Opens the store at the given path.
     * @param path Filesystem path to open.
     * @return Success or error.
     */
    [[OAT_NODISCARD("Check for error on open")]]
    virtual std::expected<void, std::error_code>
    open(std::filesystem::path const& path) = 0;

    /**
     * @brief Closes the store.
     * @return Success or error.
     */
    [[OAT_NODISCARD("Check for error on close")]]
    virtual std::expected<void, std::error_code>
    close() = 0;

    /**
     * @brief Begins a new transaction on the store.
     * @return Unique pointer to transaction_base or error.
     */
    [[OAT_NODISCARD("Check for error or valid transaction")]]
    virtual std::expected<std::unique_ptr<class transaction_base>, std::error_code>
    begin_transaction() = 0;
};


/**
 * @brief Creates a JSON file-backed store.
 * @param path Filesystem path to the JSON file.
 * @param opts Options for the JSON store.
 * @return Unique pointer to store_base or error.
 */
[[OAT_NODISCARD("Check for error or valid store")]]
ION_CORE_API std::expected<std::unique_ptr<store_base>, std::error_code>
make_json_file_store(std::filesystem::path const&, json_store_options);

/**
 * @brief Creates a TOML file-backed store.
 * @param path Filesystem path to the TOML file.
 * @param opts Options for the TOML store.
 * @return Unique pointer to store_base or error.
 */
[[OAT_NODISCARD("Check for error or valid store")]]
ION_CORE_API std::expected<std::unique_ptr<store_base>, std::error_code>
make_toml_file_store(std::filesystem::path const&, toml_store_options);

/**
 * @brief Creates an in-memory store.
 * @return Unique pointer to store_base or error.
 */
[[OAT_NODISCARD("Check for error or valid store")]]
ION_CORE_API std::expected<std::unique_ptr<store_base>, std::error_code>
make_in_memory_store();

}  // namespace ion::core
