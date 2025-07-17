#include "json_transaction_impl.h"
#include "json_store_impl.h"
#include <regex>

using namespace oxide::core;
using namespace oxide::core::detail;

JsonTransaction::JsonTransaction(nlohmann::json const& initial_data, JsonStore* store, JsonStoreOptions const& options)
    : data_(initial_data), store_(store), options_(options) {
    // Initialize root handle with empty path
    // The root handle is always 1, created here
    handle_map_[1] = Node{{}};  // Empty path for root
    next_handle_ = 2;  // Start from 2 for subsequent handles
}

JsonTransaction::~JsonTransaction() noexcept {
    if (!committed_) {
        rollback_impl();
    }
}

std::expected<StoreHandle, Error> JsonTransaction::root() const {
    return StoreHandle{1};  // Root is always handle 1
}

StoreHandle JsonTransaction::make_handle(std::vector<std::string> path) const {
    uint64_t handle = next_handle_++;
    handle_map_[handle] = Node{std::move(path)};
    return StoreHandle{handle};
}

nlohmann::json* JsonTransaction::navigate_to_node(std::vector<std::string> const& path) {
    nlohmann::json* current = &data_;
    
    for (size_t i = 0; i < path.size(); ++i) {
        const auto& segment = path[i];
        
        // Check if this is an array index
        if (i > 0 && !segment.empty() && std::all_of(segment.begin(), segment.end(), ::isdigit)) {
            if (!current->is_array()) return nullptr;
            
            size_t idx = std::stoull(segment);
            if (idx >= current->size()) return nullptr;
            
            current = &(*current)[idx];
        } else {
            // It's an object key
            if (!current->is_object()) return nullptr;
            
            auto it = current->find(segment);
            if (it == current->end()) return nullptr;
            
            current = &it.value();
        }
    }
    
    return current;
}

nlohmann::json const* JsonTransaction::navigate_to_node(std::vector<std::string> const& path) const {
    nlohmann::json const* current = &data_;
    
    for (size_t i = 0; i < path.size(); ++i) {
        const auto& segment = path[i];
        
        // Check if this is an array index
        if (i > 0 && !segment.empty() && std::all_of(segment.begin(), segment.end(), ::isdigit)) {
            if (!current->is_array()) return nullptr;
            
            size_t idx = std::stoull(segment);
            if (idx >= current->size()) return nullptr;
            
            current = &(*current)[idx];
        } else {
            // It's an object key
            if (!current->is_object()) return nullptr;
            
            auto it = current->find(segment);
            if (it == current->end()) return nullptr;
            
            current = &it.value();
        }
    }
    
    return current;
}

nlohmann::json* JsonTransaction::get_node(StoreHandle h) {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

nlohmann::json const* JsonTransaction::get_node(StoreHandle h) const {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

std::expected<nlohmann::json*, Error> JsonTransaction::get_node_checked(StoreHandle h) {
    if (h.raw == 0) {
        return std::unexpected(Error{ErrorCode::InvalidHandle, "Invalid handle"});
    }
    
    auto* node = get_node(h);
    if (!node) {
        return std::unexpected(Error{ErrorCode::InvalidHandle, "Node not found"});
    }
    
    return node;
}

std::expected<nlohmann::json const*, Error> JsonTransaction::get_node_checked(StoreHandle h) const {
    if (h.raw == 0) {
        return std::unexpected(Error{ErrorCode::InvalidHandle, "Invalid handle"});
    }
    
    auto const* node = get_node(h);
    if (!node) {
        return std::unexpected(Error{ErrorCode::InvalidHandle, "Node not found"});
    }
    
    return node;
}

bool JsonTransaction::is_valid_key(std::string_view key) const {
    if (key.empty()) return false;
    
    // Simple validation: must start with letter or underscore, then alphanumeric or underscore
    if (!std::isalpha(key[0]) && key[0] != '_') return false;
    
    for (char c : key.substr(1)) {
        if (!std::isalnum(c) && c != '_') return false;
    }
    
    return true;
}

std::expected<bool, Error> JsonTransaction::get_bool(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_boolean()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a boolean"});
    }
    
    return node->get<bool>();
}

std::expected<int64_t, Error> JsonTransaction::get_int(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_number_integer()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not an integer"});
    }
    
    return node->get<int64_t>();
}

std::expected<double, Error> JsonTransaction::get_double(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_number()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a number"});
    }
    
    return node->get<double>();
}

std::expected<std::string, Error> JsonTransaction::get_string(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_string()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a string"});
    }
    
    return node->get<std::string>();
}

std::expected<void, Error> JsonTransaction::set_bool(StoreHandle h, bool v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = v;
    return {};
}

std::expected<void, Error> JsonTransaction::set_int(StoreHandle h, int64_t v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = v;
    return {};
}

std::expected<void, Error> JsonTransaction::set_double(StoreHandle h, double v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = v;
    return {};
}

std::expected<void, Error> JsonTransaction::set_string(StoreHandle h, std::string_view v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = std::string(v);
    return {};
}

std::expected<StoreHandle, Error> JsonTransaction::make_array(StoreHandle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key format"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    // Create the array
    (*node)[std::string(key)] = nlohmann::json::array();
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<StoreHandle, Error> JsonTransaction::make_object(StoreHandle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key format"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    // Create the object
    (*node)[std::string(key)] = nlohmann::json::object();
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<void, Error> JsonTransaction::make_bool(StoreHandle parent, std::string_view key, bool v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key format"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    (*node)[std::string(key)] = v;
    return {};
}

std::expected<void, Error> JsonTransaction::make_int(StoreHandle parent, std::string_view key, int64_t v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key format"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    (*node)[std::string(key)] = v;
    return {};
}

std::expected<void, Error> JsonTransaction::make_double(StoreHandle parent, std::string_view key, double v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key format"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    (*node)[std::string(key)] = v;
    return {};
}

std::expected<void, Error> JsonTransaction::make_string(StoreHandle parent, std::string_view key, std::string_view v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key format"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    (*node)[std::string(key)] = std::string(v);
    return {};
}

std::expected<void, Error> JsonTransaction::remove(StoreHandle parent, std::string_view key) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    auto it = node->find(std::string(key));
    if (it == node->end()) {
        return std::unexpected(Error{ErrorCode::KeyNotFound, "Key not found"});
    }
    
    node->erase(it);
    return {};
}

std::expected<bool, Error> JsonTransaction::has(StoreHandle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    return node->contains(std::string(key));
}

std::expected<void, Error> JsonTransaction::erase_element(StoreHandle parent, size_t idx) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_array()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an array"});
    }
    
    if (idx >= node->size()) {
        return std::unexpected(Error{ErrorCode::IndexOutOfRange, "Index out of range"});
    }
    
    node->erase(node->begin() + idx);
    return {};
}

std::expected<bool, Error> JsonTransaction::has_element(StoreHandle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_array()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an array"});
    }
    
    return idx < node->size();
}

std::expected<StoreHandle, Error> JsonTransaction::child(StoreHandle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an object"});
    }
    
    if (!node->contains(std::string(key))) {
        return std::unexpected(Error{ErrorCode::KeyNotFound, "Key not found"});
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<StoreHandle, Error> JsonTransaction::element(StoreHandle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_array()) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an array"});
    }
    
    if (idx >= node->size()) {
        return std::unexpected(Error{ErrorCode::IndexOutOfRange, "Index out of range"});
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::to_string(idx));
    
    return make_handle(std::move(path));
}

std::expected<void, Error> JsonTransaction::commit_impl() {
    if (!store_) {
        return std::unexpected(Error{ErrorCode::InvalidState, "No store associated with transaction"});
    }
    
    auto result = store_->save_to_file(data_);
    if (result) {
        // Update store's data with the new transaction data
        store_->update_data(data_);
    }
    return result;
}

void JsonTransaction::rollback_impl() noexcept {
    // Clear the transaction data
    data_.clear();
    handle_map_.clear();
}