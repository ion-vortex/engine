#include "json_transaction_impl.h"
#include "json_store_impl.h"
#include <regex>

using namespace ion::core;
using namespace ion::core::detail;

json_transaction::json_transaction(nlohmann::json const& initial_data, json_store* store, json_store_options const& options)
    : data_(initial_data), store_(store), options_(options) {
    // Initialize root handle with empty path
    // The root handle is always 1, created here
    handle_map_[1] = node{{}};  // Empty path for root
    next_handle_ = 2;  // Start from 2 for subsequent handles
}

json_transaction::~json_transaction() noexcept {
    if (!committed_) {
        rollback_impl();
    }
}

std::expected<store_handle, std::error_code> json_transaction::root() const {
    return store_handle{1};  // Root is always handle 1
}

store_handle json_transaction::make_handle(std::vector<std::string> path) const {
    uint64_t handle = next_handle_++;
    handle_map_[handle] = node{std::move(path)};
    return store_handle{handle};
}

nlohmann::json* json_transaction::navigate_to_node(std::vector<std::string> const& path) {
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

nlohmann::json const* json_transaction::navigate_to_node(std::vector<std::string> const& path) const {
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

nlohmann::json* json_transaction::get_node(store_handle h) {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

nlohmann::json const* json_transaction::get_node(store_handle h) const {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

std::expected<nlohmann::json*, std::error_code> json_transaction::get_node_checked(store_handle h) {
    if (h.raw == 0) {
        return std::unexpected(make_error_code(core_errc::invalid_handle));
    }
    
    auto* node = get_node(h);
    if (!node) {
        return std::unexpected(make_error_code(core_errc::invalid_handle));
    }
    
    return node;
}

std::expected<nlohmann::json const*, std::error_code> json_transaction::get_node_checked(store_handle h) const {
    if (h.raw == 0) {
        return std::unexpected(make_error_code(core_errc::invalid_handle));
    }
    
    auto const* node = get_node(h);
    if (!node) {
        return std::unexpected(make_error_code(core_errc::invalid_handle));
    }
    
    return node;
}

bool json_transaction::is_valid_key(std::string_view key) const {
    if (key.empty()) return false;
    
    // Simple validation: must start with letter or underscore, then alphanumeric or underscore
    if (!std::isalpha(key[0]) && key[0] != '_') return false;
    
    for (char c : key.substr(1)) {
        if (!std::isalnum(c) && c != '_') return false;
    }
    
    return true;
}

std::expected<bool, std::error_code> json_transaction::get_bool(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_boolean()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return node->get<bool>();
}

std::expected<int64_t, std::error_code> json_transaction::get_int(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_number_integer()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return node->get<int64_t>();
}

std::expected<double, std::error_code> json_transaction::get_double(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_number()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return node->get<double>();
}

std::expected<std::string, std::error_code> json_transaction::get_string(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_string()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return node->get<std::string>();
}

std::expected<void, std::error_code> json_transaction::set_bool(store_handle h, bool v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = v;
    return {};
}

std::expected<void, std::error_code> json_transaction::set_int(store_handle h, int64_t v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = v;
    return {};
}

std::expected<void, std::error_code> json_transaction::set_double(store_handle h, double v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = v;
    return {};
}

std::expected<void, std::error_code> json_transaction::set_string(store_handle h, std::string_view v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    *node = std::string(v);
    return {};
}

std::expected<store_handle, std::error_code> json_transaction::make_array(store_handle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    // Create the array
    (*node)[std::string(key)] = nlohmann::json::array();
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<store_handle, std::error_code> json_transaction::make_object(store_handle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    // Create the object
    (*node)[std::string(key)] = nlohmann::json::object();
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<void, std::error_code> json_transaction::make_bool(store_handle parent, std::string_view key, bool v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    (*node)[std::string(key)] = v;
    return {};
}

std::expected<void, std::error_code> json_transaction::make_int(store_handle parent, std::string_view key, int64_t v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    (*node)[std::string(key)] = v;
    return {};
}

std::expected<void, std::error_code> json_transaction::make_double(store_handle parent, std::string_view key, double v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    (*node)[std::string(key)] = v;
    return {};
}

std::expected<void, std::error_code> json_transaction::make_string(store_handle parent, std::string_view key, std::string_view v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    (*node)[std::string(key)] = std::string(v);
    return {};
}

std::expected<void, std::error_code> json_transaction::remove(store_handle parent, std::string_view key) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    auto it = node->find(std::string(key));
    if (it == node->end()) {
        return std::unexpected(make_error_code(core_errc::key_not_found));
    }
    
    node->erase(it);
    return {};
}

std::expected<bool, std::error_code> json_transaction::has(store_handle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return node->contains(std::string(key));
}

std::expected<void, std::error_code> json_transaction::erase_element(store_handle parent, size_t idx) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* node = *node_result;
    if (!node->is_array()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (idx >= node->size()) {
        return std::unexpected(make_error_code(core_errc::index_out_of_range));
    }
    
    node->erase(node->begin() + idx);
    return {};
}

std::expected<bool, std::error_code> json_transaction::has_element(store_handle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_array()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return idx < node->size();
}

std::expected<store_handle, std::error_code> json_transaction::child(store_handle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_object()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (!node->contains(std::string(key))) {
        return std::unexpected(make_error_code(core_errc::key_not_found));
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<store_handle, std::error_code> json_transaction::element(store_handle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto const* node = *node_result;
    if (!node->is_array()) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (idx >= node->size()) {
        return std::unexpected(make_error_code(core_errc::index_out_of_range));
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::to_string(idx));
    
    return make_handle(std::move(path));
}

std::expected<void, std::error_code> json_transaction::commit_impl() {
    if (!store_) {
        return std::unexpected(make_error_code(core_errc::invalid_state));
    }
    
    auto result = store_->save_to_file(data_);
    if (result) {
        // Update store's data with the new transaction data
        store_->update_data(data_);
    }
    return result;
}

void json_transaction::rollback_impl() noexcept {
    // Clear the transaction data
    data_.clear();
    handle_map_.clear();
}