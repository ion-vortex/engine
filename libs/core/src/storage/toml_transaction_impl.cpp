#include "toml_transaction_impl.h"
#include "toml_store_impl.h"
#include <regex>

using namespace ion::core;
using namespace ion::core::detail;

toml_transaction::toml_transaction(toml::table const& initial_data, toml_store* store, toml_store_options const& options)
    : data_(initial_data), store_(store), options_(options) {
    // Initialize root handle with empty path
    // The root handle is always 1, created here
    handle_map_[1] = Node{{}};  // Empty path for root
    next_handle_ = 2;  // Start from 2 for subsequent handles
}

toml_transaction::~toml_transaction() noexcept {
    if (!committed_) {
        rollback_impl();
    }
}

std::expected<store_handle, std::error_code> toml_transaction::root() const {
    return store_handle{1};  // Root is always handle 1
}

store_handle toml_transaction::make_handle(std::vector<std::string> path) const {
    uint64_t handle = next_handle_++;
    handle_map_[handle] = Node{std::move(path)};
    return store_handle{handle};
}

toml::node* toml_transaction::navigate_to_node(std::vector<std::string> const& path) {
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

toml::node const* toml_transaction::navigate_to_node(std::vector<std::string> const& path) const {
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

toml::node* toml_transaction::get_node(store_handle h) {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

toml::node const* toml_transaction::get_node(store_handle h) const {
    auto it = handle_map_.find(h.raw);
    if (it == handle_map_.end()) return nullptr;
    
    return navigate_to_node(it->second.path);
}

std::expected<toml::node*, std::error_code> toml_transaction::get_node_checked(store_handle h) {
    if (!h.valid()) {
        return std::unexpected(make_error_code(core_errc::invalid_argument));
    }
    
    auto* node = get_node(h);
    if (!node) {
        return std::unexpected(make_error_code(core_errc::key_not_found));
    }
    
    return node;
}

std::expected<toml::node const*, std::error_code> toml_transaction::get_node_checked(store_handle h) const {
    if (!h.valid()) {
        return std::unexpected(make_error_code(core_errc::invalid_argument));
    }
    
    auto const* node = get_node(h);
    if (!node) {
        return std::unexpected(make_error_code(core_errc::key_not_found));
    }
    
    return node;
}

bool toml_transaction::is_valid_key(std::string_view key) const {
    static const std::regex key_pattern("[A-Za-z_][A-Za-z0-9_]*");
    return std::regex_match(key.begin(), key.end(), key_pattern);
}

std::expected<bool, std::error_code> toml_transaction::get_bool(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_boolean();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return val->get();
}

std::expected<int64_t, std::error_code> toml_transaction::get_int(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_integer();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return val->get();
}

std::expected<double, std::error_code> toml_transaction::get_double(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_floating_point();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return val->get();
}

std::expected<std::string, std::error_code> toml_transaction::get_string(store_handle h) const {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_string();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return std::string(val->get());
}

std::expected<void, std::error_code> toml_transaction::set_bool(store_handle h, bool v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_boolean();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    *val = v;
    return {};
}

std::expected<void, std::error_code> toml_transaction::set_int(store_handle h, int64_t v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_integer();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    *val = v;
    return {};
}

std::expected<void, std::error_code> toml_transaction::set_double(store_handle h, double v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_floating_point();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    *val = v;
    return {};
}

std::expected<void, std::error_code> toml_transaction::set_string(store_handle h, std::string_view v) {
    auto node_result = get_node_checked(h);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* val = (*node_result)->as_string();
    if (!val) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    *val = v;
    return {};
}

std::expected<store_handle, std::error_code> toml_transaction::make_array(store_handle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::already_exists));
    }
    
    table->insert(key, toml::array{});
    
    // Build path by looking up parent's path
    auto it = handle_map_.find(parent.raw);
    auto path = it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<store_handle, std::error_code> toml_transaction::make_object(store_handle parent, std::string_view key) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::already_exists));
    }
    
    table->insert(key, toml::table{});
    
    // Build path by looking up parent's path
    auto it = handle_map_.find(parent.raw);
    auto path = it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<void, std::error_code> toml_transaction::make_bool(store_handle parent, std::string_view key, bool v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::already_exists));
    }
    
    table->insert(key, v);
    return {};
}

std::expected<void, std::error_code> toml_transaction::make_int(store_handle parent, std::string_view key, int64_t v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::already_exists));
    }
    
    table->insert(key, v);
    return {};
}

std::expected<void, std::error_code> toml_transaction::make_double(store_handle parent, std::string_view key, double v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::already_exists));
    }
    
    table->insert(key, v);
    return {};
}

std::expected<void, std::error_code> toml_transaction::make_string(store_handle parent, std::string_view key, std::string_view v) {
    if (!is_valid_key(key)) {
        return std::unexpected(make_error_code(core_errc::path_syntax));
    }
    
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::already_exists));
    }
    
    table->insert(key, std::string(v));
    return {};
}

std::expected<void, std::error_code> toml_transaction::remove(store_handle parent, std::string_view key) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (!table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::key_not_found));
    }
    
    table->erase(key);
    return {};
}

std::expected<bool, std::error_code> toml_transaction::has(store_handle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return table->contains(key);
}

std::expected<void, std::error_code> toml_transaction::erase_element(store_handle parent, size_t idx) {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* array = (*node_result)->as_array();
    if (!array) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (idx >= array->size()) {
        return std::unexpected(make_error_code(core_errc::index_out_of_range));
    }
    
    array->erase(array->begin() + idx);
    return {};
}

std::expected<bool, std::error_code> toml_transaction::has_element(store_handle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* array = (*node_result)->as_array();
    if (!array) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    return idx < array->size();
}

std::expected<store_handle, std::error_code> toml_transaction::child(store_handle parent, std::string_view key) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* table = (*node_result)->as_table();
    if (!table) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (!table->contains(key)) {
        return std::unexpected(make_error_code(core_errc::key_not_found));
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::string(key));
    
    return make_handle(std::move(path));
}

std::expected<store_handle, std::error_code> toml_transaction::element(store_handle parent, size_t idx) const {
    auto node_result = get_node_checked(parent);
    if (!node_result) return std::unexpected(node_result.error());
    
    auto* array = (*node_result)->as_array();
    if (!array) {
        return std::unexpected(make_error_code(core_errc::type_mismatch));
    }
    
    if (idx >= array->size()) {
        return std::unexpected(make_error_code(core_errc::index_out_of_range));
    }
    
    // Build path by looking up parent's path
    auto parent_it = handle_map_.find(parent.raw);
    auto path = parent_it->second.path;
    path.push_back(std::to_string(idx));
    
    return make_handle(std::move(path));
}

std::expected<void, std::error_code> toml_transaction::commit_impl() {
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

void toml_transaction::rollback_impl() noexcept {
    // Clear the transaction data
    data_.clear();
    handle_map_.clear();
}
