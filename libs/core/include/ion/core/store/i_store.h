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
struct ION_CORE_API FileStoreOptions {
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
struct ION_CORE_API JsonStoreOptions {
    bool write_mmap     = false;   ///< Use memory-mapped writes if true.
    bool use_journal    = true;    ///< Enable journaling for crash safety.
    bool allow_comments = false;   ///< Allow comments in JSON files.
};


/**
 * @brief Options for TOML file stores.
 *
 * Controls memory-mapping, journaling, key order, and type strictness for TOML backends.
 */
struct ION_CORE_API TomlStoreOptions {
    bool write_mmap     = false;   ///< Use memory-mapped writes if true.
    bool use_journal    = true;    ///< Enable journaling for crash safety.
    bool preserve_order = false;   ///< Preserve key order in TOML files.
    bool strict_types   = true;    ///< Enforce strict TOML type rules.
};


/**
 * @brief Abstract interface for a transactional storage backend.
 *
 * @note Thread-safety: neither IStore nor ITransaction are thread-safe. Concurrent access must be synchronized externally.
 *
 * @note Path rules: Keys in path-strings must match `[A-Za-z_][A-Za-z0-9_]*`. No quoting/escaping is supported; invalid segments yield PathSyntax.
 *
 * Provides methods to open/close the store and begin transactions.
 */
class ION_CORE_API IStore {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IStore() = default;

    /**
     * @brief Opens the store at the given path.
     * @param path Filesystem path to open.
     * @return Success or error.
     */
    [[nodiscard("Check for error on open"), gnu::warn_unused_result]]
    virtual std::expected<void, Error>
    open(std::filesystem::path const& path) = 0;

    /**
     * @brief Closes the store.
     * @return Success or error.
     */
    [[nodiscard("Check for error on close"), gnu::warn_unused_result]]
    virtual std::expected<void, Error>
    close() = 0;

    /**
     * @brief Begins a new transaction on the store.
     * @return Unique pointer to ITransaction or error.
     */
    [[nodiscard("Check for error or valid transaction"), gnu::warn_unused_result]]
    virtual std::expected<std::unique_ptr<class ITransaction>, Error>
    begin_transaction() = 0;
};


/**
 * @brief Creates a JSON file-backed store.
 * @param path Filesystem path to the JSON file.
 * @param opts Options for the JSON store.
 * @return Unique pointer to IStore or error.
 */
[[nodiscard("Check for error or valid store"), gnu::warn_unused_result]]
ION_CORE_API std::expected<std::unique_ptr<IStore>, Error>
make_json_file_store(std::filesystem::path const&, JsonStoreOptions);

/**
 * @brief Creates a TOML file-backed store.
 * @param path Filesystem path to the TOML file.
 * @param opts Options for the TOML store.
 * @return Unique pointer to IStore or error.
 */
[[nodiscard("Check for error or valid store"), gnu::warn_unused_result]]
ION_CORE_API std::expected<std::unique_ptr<IStore>, Error>
make_toml_file_store(std::filesystem::path const&, TomlStoreOptions);

/**
 * @brief Creates an in-memory store.
 * @return Unique pointer to IStore or error.
 */
[[nodiscard("Check for error or valid store"), gnu::warn_unused_result]]
ION_CORE_API std::expected<std::unique_ptr<IStore>, Error>
make_in_memory_store();

}  // namespace ion::core
