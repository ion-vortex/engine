#include "toml_transaction_impl.h"
#include "toml_store_impl.h"
#include <regex>

using namespace ion::core;
using namespace ion::core::detail;

TomlTransaction::TomlTransaction(toml::table const& initial_data, TomlStore* store, TomlStoreOptions const& options)
    : data_(initial_data), store_(store), options_(options) {
    // Initialize root handle with empty path
    // The root handle is always 1, created here
    handle_map_[1] = Node{{}};  // Empty path for root
    next_handle_ = 2;  // Start from 2 for subsequent handles
}

TomlTransaction::~TomlTransaction() noexcept {
    if (!committed_) {
        rollback_impl();
    }
}

std::expected<StoreHandle, Error> TomlTransaction::root() const {
    return StoreHandle{1};  // Root is always handle 1
}

StoreHandle TomlTransaction::make_handle(std::vector<std::string> path) const {
    uint64_t handle = next_handle_++;
    handle_map_[handle] = Node{std::move(path)};
    return StoreHandle{handle};
}

toml::node* TomlTransaction::navigate_to_node(std::vector<std::string> const& path) {
    toml::node* current = &data_;
    
    for (size_t i = 0; i < path.size(); ++i) {
        const auto& segment = path[i];
        
        // Check if this is an array index
        if (i > 0 && !segment.empty() && std::all_of(segment.begin(), segment.end(), ::isdigit)) {
            auto* array = current->as_array();
            if (!array) return nullptr;
            
            size_t idx = std::stoull(segment);
            if (idx >= array->size()) return nullptr;
            
            current = &(*array)[idx];
        } else {
            // It's a table key
            auto* table = current->as_table();
            if (!table) return nullptr;
            
            auto it = table->find(segment);
            if (it == table->end()) return nullptr;
            
            current = &it->second;
        }
    }
    
    return current;
}

toml::node const* TomlTransaction::navigate_to_node(std::vector<std::string> const& path) const {
    toml::node const* current = &data_;
    
    for (size_t i = 0; i < path.size(); ++i) {
        const auto& segment = path[i];
        
        // Check if this is an array index
        if (i > 0 && !segment.empty() && std::all_of(segment.begin(), segment.end(), ::isdigit)) {
            auto const* array = current->as_array();
            if (!array) return nullptr;
            
            size_t idx = std::stoull(segment);
            if (idx >= array->size()) return nullptr;
            
            current = &(*array)[idx];
        } else {
            // It's a table key
            auto const* table = current->as_table();
            if (!table) return nullptr;
            
            auto it = table->find(segment);
            if (it == table->end()) return nullptr;
            
            current = &it->second;
        }
    }
    
    return current;
}

toml::node* TomlTransaction::get_node(StoreHandle h) {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

toml::node const* TomlTransaction::get_node(StoreHandle h) const {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

std::expected<toml::node*, Error> TomlTransaction::get_node_checked(StoreHandle h) {
    if (!h.valid()) {
        return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid handle"});
    }
    
    auto* node = get_node(h);
    if (!node) {
        return std::unexpected(Error{ErrorCode::KeyNotFound, "Handle not found or path invalid"});
    }
    
    return node;
}

std::expected<toml::node const*, Error> TomlTransaction::get_node_checked(StoreHandle h) const {
    if (!h.valid()) {
        return std::unexpected(Error{ErrorCode::InvalidArgument, "Invalid handle"});
    }
    
    auto const* node = get_node(h);
    if (!node) {
        return std::unexpected(Error{ErrorCode::KeyNotFound, "Handle not found or path invalid"});
    }
    
    return node;
}

bool TomlTransaction::is_valid_key(std::string_view key) const {
    static const std::regex key_pattern("[A-Za-z_][A-Za-z0-9_]*");
    return std::regex_match(key.begin(), key.end(), key_pattern);
}

std::expected<bool, Error> TomlTransaction::get_bool(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_boolean();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a boolean"});
    }
    
    return val->get();
}

std::expected<int64_t, Error> TomlTransaction::get_int(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_integer();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not an integer"});
    }
    
    return val->get();
}

std::expected<double, Error> TomlTransaction::get_double(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_floating_point();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a floating point"});
    }
    
    return val->get();
}

std::expected<std::string, Error> TomlTransaction::get_string(StoreHandle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_string();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a string"});
    }
    
    return std::string(val->get());
}

std::expected<void, Error> TomlTransaction::set_bool(StoreHandle h, bool v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_boolean();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a boolean"});
    }
    
    *val = v;
    return {};
}

std::expected<void, Error> TomlTransaction::set_int(StoreHandle h, int64_t v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_integer();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not an integer"});
    }
    
    *val = v;
    return {};
}

std::expected<void, Error> TomlTransaction::set_double(StoreHandle h, double v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_floating_point();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a floating point"});
    }
    
    *val = v;
    return {};
}

std::expected<void, Error> TomlTransaction::set_string(StoreHandle h, std::string_view v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_string();
    if (!val) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Node is not a string"});
    }
    
    *val = v;
    return {};
}

std::expected<StoreHandle, Error> TomlTransaction::make_array(StoreHandle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key name"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (table->contains(key)) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Key already exists"});
    }
    
    table->insert(key, toml::array{});
    
    // Build path by looking up parent's path
    auto it = handle_map_.find(parent.raw);
    auto path = it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<StoreHandle, Error> TomlTransaction::make_object(StoreHandle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key name"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (table->contains(key)) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Key already exists"});
    }
    
    table->insert(key, toml::table{});
    
    // Build path by looking up parent's path
    auto it = handle_map_.find(parent.raw);
    auto path = it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<void, Error> TomlTransaction::make_bool(StoreHandle parent, std::string_view key, bool v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key name"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (table->contains(key)) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Key already exists"});
    }
    
    table->insert(key, v);
    return {};
}

std::expected<void, Error> TomlTransaction::make_int(StoreHandle parent, std::string_view key, int64_t v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key name"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (table->contains(key)) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Key already exists"});
    }
    
    table->insert(key, v);
    return {};
}

std::expected<void, Error> TomlTransaction::make_double(StoreHandle parent, std::string_view key, double v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key name"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (table->contains(key)) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Key already exists"});
    }
    
    table->insert(key, v);
    return {};
}

std::expected<void, Error> TomlTransaction::make_string(StoreHandle parent, std::string_view key, std::string_view v) {
    if (!is_valid_key(key)) {
        return std::unexpected(Error{ErrorCode::PathSyntax, "Invalid key name"});
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (table->contains(key)) {
        return std::unexpected(Error{ErrorCode::AlreadyExists, "Key already exists"});
    }
    
    table->insert(key, std::string(v));
    return {};
}

std::expected<void, Error> TomlTransaction::remove(StoreHandle parent, std::string_view key) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (!table->contains(key)) {
        return std::unexpected(Error{ErrorCode::KeyNotFound, "Key not found"});
    }
    
    table->erase(key);
    return {};
}

std::expected<bool, Error> TomlTransaction::has(StoreHandle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    return table->contains(key);
}

std::expected<void, Error> TomlTransaction::erase_element(StoreHandle parent, size_t idx) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* array = (*node_result)->as_array();
    if (!array) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an array"});
    }
    
    if (idx >= array->size()) {
        return std::unexpected(Error{ErrorCode::IndexOutOfRange, "Index out of range"});
    }
    
    array->erase(array->begin() + idx);
    return {};
}

std::expected<bool, Error> TomlTransaction::has_element(StoreHandle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* array = (*node_result)->as_array();
    if (!array) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an array"});
    }
    
    return idx < array->size();
}

std::expected<StoreHandle, Error> TomlTransaction::child(StoreHandle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not a table"});
    }
    
    if (!table->contains(key)) {
        return std::unexpected(Error{ErrorCode::KeyNotFound, "Key not found"});
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<StoreHandle, Error> TomlTransaction::element(StoreHandle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* array = (*node_result)->as_array();
    if (!array) {
        return std::unexpected(Error{ErrorCode::TypeMismatch, "Parent is not an array"});
    }
    
    if (idx >= array->size()) {
        return std::unexpected(Error{ErrorCode::IndexOutOfRange, "Index out of range"});
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::to_string(idx));
    
    return make_handle(std::move(path));
}

std::expected<void, Error> TomlTransaction::commit_impl() {
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

void TomlTransaction::rollback_impl() noexcept {
    // Clear the transaction data
    data_.clear();
    handle_map_.clear();
}
