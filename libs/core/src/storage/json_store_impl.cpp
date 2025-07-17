/**
 * @file json_store_impl.cpp
 * @brief Implementation of the JsonStore class for managing JSON-based storage.
 *
 * This file contains the implementation of the JsonStore class, which provides
 * methods for opening, closing, and managing transactions on a JSON-based
 * storage backend. The class ensures ACID compliance by using atomic file
 * operations for saving data.
 */

#include "json_store_impl.h"
#include "json_transaction_impl.h"
#include <fstream>
#include <sstream>

using namespace oxide::core;
using namespace oxide::core::detail;

/**
 * @brief Constructs a JsonStore instance.
 * @param path Filesystem path to the JSON file.
 * @param options Options for configuring the JSON store.
 */
JsonStore::JsonStore(std::filesystem::path const& path, JsonStoreOptions const& options)
    : path_(path), options_(options), data_(nlohmann::json::object()) { }

/**
 * @brief Destructor for JsonStore.
 *
 * Ensures the store is properly closed if it is still open.
 */
JsonStore::~JsonStore() {
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
 * @brief Opens the JSON store at the specified path.
 * @param path Filesystem path to the JSON file.
 * @return Success or an error if the store is already open or the file cannot be read.
 */
std::expected<void, Error> JsonStore::open(std::filesystem::path const& path) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (is_open_) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Store already open"});
    }

    path_ = path;

    // Load an existing file or create an empty object.
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
 * @brief Closes the JSON store.
 *
 * Clears the in-memory data and marks the store as closed.
 * @return Success or an error if the store is not open.
 */
std::expected<void, Error> JsonStore::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!is_open_) {
        return std::unexpected(Error{ErrorCode::InvalidState, "Store not open"});
    }

    is_open_ = false;
    data_.clear(); // Clear in-memory data
    return {};
}

/**
 * @brief Begins a new transaction on the JSON store.
 *
 * Creates a new JsonTransaction instance for managing changes to the store.
 * @return A unique pointer to the transaction or an error if the store is not open.
 */
std::expected<std::unique_ptr<ITransaction>, Error> JsonStore::begin_transaction() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!is_open_) {
        return std::unexpected(Error{ErrorCode::InvalidState, "Store not open"});
    }

    auto txn = std::make_unique<JsonTransaction>(data_, this, options_);
    return txn;
}

/**
 * @brief Loads the JSON data from the file into memory.
 *
 * Parses the JSON file and populates the in-memory representation.
 * @return Success or an error if the file cannot be read or parsed.
 */
std::expected<void, Error> JsonStore::load_from_file() {
    try {
        std::ifstream file(path_, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            return std::unexpected(Error{ErrorCode::IoFailure, "Failed to open file for reading"});
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        
        auto content = buffer.str();
        if (content.empty()) {
            // Empty file, use empty object
            data_ = nlohmann::json::object();
        } else {
            data_ = nlohmann::json::parse(content, nullptr, options_.allow_comments);
        }

        return {};
    } catch (const nlohmann::json::parse_error& e) {
        return std::unexpected(Error{ErrorCode::ParseError, e.what()});
    } catch (const std::exception& e) {
        return std::unexpected(Error{ErrorCode::Unknown, e.what()});
    } catch (...) {
        return std::unexpected(Error{ErrorCode::Unknown, "Unknown error occurred while loading JSON file"});
    }
}

/**
 * @brief Saves the JSON data to the file.
 *
 * Writes the in-memory JSON data to a temporary file and atomically renames it
 * to replace the original file.
 * @param data The JSON data to save.
 * @return Success or an error if the file cannot be written.
 */
std::expected<void, Error> JsonStore::save_to_file(nlohmann::json const& data) {
    try {
        // Write to a temporary file for atomicity
        std::filesystem::path temp_path = path_;
        temp_path += ".tmp";

        {
            std::ofstream temp_file(temp_path, std::ios::out | std::ios::trunc);
            if (!temp_file.is_open()) {
                return std::unexpected(Error{ErrorCode::IoFailure, "Failed to open temporary file"});
            }
            
            // Pretty print with 2-space indentation
            temp_file << data.dump(2);
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
        return std::unexpected(Error{ErrorCode::Unknown, "Unknown error occurred while saving JSON file"});
    }
}