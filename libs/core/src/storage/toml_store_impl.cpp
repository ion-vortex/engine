/**
 * @file toml_store_impl.cpp
 * @brief Implementation of the TomlStore class for managing TOML-based storage.
 *
 * This file contains the implementation of the TomlStore class, which provides
 * methods for opening, closing, and managing transactions on a TOML-based
 * storage backend. The class ensures ACID compliance by using atomic file
 * operations for saving data.
 */

#include "toml_store_impl.h"
#include "toml_transaction_impl.h"
#include <fstream>
#include <sstream>

using namespace oxide::core;
using namespace oxide::core::detail;

/**
 * @brief Constructs a TomlStore instance.
 * @param path Filesystem path to the TOML file.
 * @param options Options for configuring the TOML store.
 */
TomlStore::TomlStore(std::filesystem::path const& path, TomlStoreOptions const& options)
    : path_(path), options_(options), data_() { }

/**
 * @brief Destructor for TomlStore.
 *
 * Ensures the store is properly closed if it is still open.
 */
TomlStore::~TomlStore() {
    if (is_open_) {
        auto result = close();
        if (!result) {
            // Not much we can do here if closing fails,
            // but we should log or handle the error in a real application.
            // For now, we just ignore it.
        }
    }
}

/**
 * @brief Opens the TOML store at the specified path.
 * @param path Filesystem path to the TOML file.
 * @return Success or an error if the store is already open or the file cannot be read.
 */
std::expected<void, Error> TomlStore::open(std::filesystem::path const& path) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (is_open_) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Store already open"});
    }

    path_ = path;

    // Load an existing file or create an empty table.
    if (std::filesystem::exists(path_)) {
        auto result = load_from_file();
        if (!result) {
            return result; // Propagate error from load
        }
    }

    is_open_ = true;
    return {};
}

/**
 * @brief Closes the TOML store.
 *
 * Clears the in-memory data and marks the store as closed.
 * @return Success or an error if the store is not open.
 */
std::expected<void, Error> TomlStore::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!is_open_) {
        return std::unexpected(Error{ErrorCode::InvalidState, "Store not open"});
    }

    is_open_ = false;
    data_.clear(); // Clear in-memory data
    return {};
}

/**
 * @brief Begins a new transaction on the TOML store.
 *
 * Creates a new TomlTransaction instance for managing changes to the store.
 * @return A unique pointer to the transaction or an error if the store is not open.
 */
std::expected<std::unique_ptr<ITransaction>, Error> TomlStore::begin_transaction() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!is_open_) {
        return std::unexpected(Error{ErrorCode::InvalidState, "Store not open"});
    }

    auto txn = std::make_unique<TomlTransaction>(data_, this, options_);
    return txn;
}

/**
 * @brief Loads the TOML data from the file into memory.
 *
 * Parses the TOML file and populates the in-memory representation.
 * @return Success or an error if the file cannot be read or parsed.
 */
std::expected<void, Error> TomlStore::load_from_file() {
    try {
        std::ifstream file(path_, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return std::unexpected(Error{ErrorCode::IoFailure, "Failed to open file for reading"});
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        auto result = toml::parse(buffer.str());
        data_ = std::move(result);

        return {};
    } catch (const toml::parse_error& e) {
        return std::unexpected(Error{ErrorCode::ParseError, e.description()});
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::Unknown, e.what()});
    } catch (...) {
        return std::unexpected(Error{ErrorCode::Unknown, "Unknown error occurred while loading TOML file"});
    }
}

/**
 * @brief Saves the TOML data to the file.
 *
 * Writes the in-memory TOML data to a temporary file and atomically renames it
 * to replace the original file.
 * @param data The TOML data to save.
 * @return Success or an error if the file cannot be written.
 */
std::expected<void, Error> TomlStore::save_to_file(toml::table const& data) {
    try {
        // Write to a temporary file for atomicity
        std::filesystem::path temp_path = path_;
        temp_path += ".tmp";

        {
            std::ofstream temp_file(temp_path, std::ios::out | std::ios::trunc);
            if (!temp_file.is_open()) {
                return std::unexpected(Error{ErrorCode::IoFailure, "Failed to open temporary file"});
            }
            
            temp_file << data;
            temp_file.flush();

            if (!temp_file.good()) {
                return std::unexpected(Error{ErrorCode::IoFailure, "Failed to write to temporary file"});
            }
        }

        // Atomic rename to replace the original file
        std::filesystem::rename(temp_path, path_);
        
        return {};
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::IoFailure, e.what()});
    } catch (...) {
        return std::unexpected(Error{ErrorCode::Unknown, "Unknown error occurred while saving TOML file"});
    }
}