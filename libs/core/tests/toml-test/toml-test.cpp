#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/catch_approx.hpp>
#include <oxide/core/store.h>
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

using namespace oxide::core;
using namespace Catch::Matchers;
namespace fs = std::filesystem;

class TempFile {
    fs::path path_;
public:
    TempFile(const std::string& name) 
        : path_(fs::temp_directory_path() / name) {
        cleanup();
    }
    
    ~TempFile() { cleanup(); }
    
    const fs::path& path() const { return path_; }
    
    void write(const std::string& content) {
        std::ofstream f(path_);
        f << content;
    }
    
    std::string read() const {
        std::ifstream f(path_);
        return std::string(std::istreambuf_iterator<char>(f), {});
    }
    
    bool exists() const { return fs::exists(path_); }
    
private:
    void cleanup() {
        if (fs::exists(path_)) {
            fs::remove(path_);
        }
    }
};

TEST_CASE("TOML Store - Basic Operations", "[storage][toml]") {
    TempFile temp("test_basic.toml");
    TomlStoreOptions opts{};
    
    SECTION("Create and open empty store") {
        auto store_result = make_toml_file_store(temp.path(), opts);
        REQUIRE(store_result.has_value());
        auto& store = *store_result;
        
        auto open_result = store->open(temp.path());
        REQUIRE(open_result.has_value());
        
        auto close_result = store->close();
        REQUIRE(close_result.has_value());
    }
    
    SECTION("Open non-existent file creates it") {
        REQUIRE_FALSE(temp.exists());
        
        auto store_result = make_toml_file_store(temp.path(), opts);
        REQUIRE(store_result.has_value());
        auto& store = *store_result;
        
        auto open_result = store->open(temp.path());
        REQUIRE(open_result.has_value());
        
        // Begin transaction to trigger file creation
        auto txn_result = store->begin_transaction();
        REQUIRE(txn_result.has_value());
        auto& txn = *txn_result;
        
        auto commit_result = txn->commit();
        REQUIRE(commit_result.has_value());
        
        REQUIRE(temp.exists());
    }
    
    SECTION("Cannot open store twice") {
        auto store_result = make_toml_file_store(temp.path(), opts);
        REQUIRE(store_result.has_value());
        auto& store = *store_result;
        
        auto open1 = store->open(temp.path());
        REQUIRE(open1.has_value());
        
        auto open2 = store->open(temp.path());
        REQUIRE_FALSE(open2.has_value());
        REQUIRE(open2.error().code == ErrorCode::AlreadyExists);
    }
    
    SECTION("Cannot begin transaction on closed store") {
        auto store_result = make_toml_file_store(temp.path(), opts);
        REQUIRE(store_result.has_value());
        auto& store = *store_result;
        
        // Don't open the store
        auto txn_result = store->begin_transaction();
        REQUIRE_FALSE(txn_result.has_value());
        REQUIRE(txn_result.error().code == ErrorCode::InvalidState);
    }
}

TEST_CASE("TOML Transaction - ACID Properties", "[storage][toml][acid]") {
    TempFile temp("test_acid.toml");
    TomlStoreOptions opts{};
    
    auto store_result = make_toml_file_store(temp.path(), opts);
    REQUIRE(store_result.has_value());
    auto& store = *store_result;
    
    auto open_result = store->open(temp.path());
    REQUIRE(open_result.has_value());
    
    SECTION("Atomicity - All or nothing") {
        // First transaction - create some data
        {
            auto txn_result = store->begin_transaction();
            REQUIRE(txn_result.has_value());
            auto& txn = *txn_result;
            
            auto root = txn->root();
            REQUIRE(root.has_value());
            
            auto result1 = txn->make_string(*root, "key1", "value1");
            REQUIRE(result1.has_value());
            
            auto result2 = txn->make_int(*root, "key2", 42);
            REQUIRE(result2.has_value());
            
            auto commit = txn->commit();
            REQUIRE(commit.has_value());
        }
        
        // Second transaction - try to add more but don't commit
        {
            auto txn_result = store->begin_transaction();
            REQUIRE(txn_result.has_value());
            auto& txn = *txn_result;
            
            auto root = txn->root();
            REQUIRE(root.has_value());
            
            auto result3 = txn->make_string(*root, "key3", "value3");
            REQUIRE(result3.has_value());
            
            // Destructor should rollback
        }
        
        // Third transaction - verify only committed data exists
        {
            auto txn_result = store->begin_transaction();
            REQUIRE(txn_result.has_value());
            auto& txn = *txn_result;
            
            auto root = txn->root();
            REQUIRE(root.has_value());
            
            auto has1 = txn->has(*root, "key1");
            REQUIRE(has1.has_value());
            REQUIRE(*has1 == true);
            
            auto has2 = txn->has(*root, "key2");
            REQUIRE(has2.has_value());
            REQUIRE(*has2 == true);
            
            auto has3 = txn->has(*root, "key3");
            REQUIRE(has3.has_value());
            REQUIRE(*has3 == false);
        }
    }
    
    SECTION("Consistency - Type safety") {
        auto txn_result = store->begin_transaction();
        REQUIRE(txn_result.has_value());
        auto& txn = *txn_result;
        
        auto root = txn->root();
        REQUIRE(root.has_value());
        
        // Create a string value
        auto make_result = txn->make_string(*root, "str_key", "hello");
        REQUIRE(make_result.has_value());
        
        auto str_handle_result = txn->child(*root, "str_key");
        REQUIRE(str_handle_result.has_value());
        
        // Try to read it as different types
        auto str_result = txn->get_string(*str_handle_result);
        REQUIRE(str_result.has_value());
        REQUIRE(*str_result == "hello");
        
        auto int_result = txn->get_int(*str_handle_result);
        REQUIRE_FALSE(int_result.has_value());
        REQUIRE(int_result.error().code == ErrorCode::TypeMismatch);
        
        auto bool_result = txn->get_bool(*str_handle_result);
        REQUIRE_FALSE(bool_result.has_value());
        REQUIRE(bool_result.error().code == ErrorCode::TypeMismatch);
    }
    
    SECTION("Isolation - Transactions don't interfere") {
        // This is a simplified test since we don't support concurrent transactions
        // But we can test that each transaction starts with a consistent view
        
        // First transaction
        {
            auto txn1 = store->begin_transaction();
            REQUIRE(txn1.has_value());
            
            auto root = (*txn1)->root();
            REQUIRE(root.has_value());
            
            auto result = (*txn1)->make_int(*root, "counter", 1);
            REQUIRE(result.has_value());
            
            auto commit = (*txn1)->commit();
            REQUIRE(commit.has_value());
        }
        
        // Second transaction sees committed value
        {
            auto txn2 = store->begin_transaction();
            REQUIRE(txn2.has_value());
            
            auto root = (*txn2)->root();
            REQUIRE(root.has_value());
            
            auto handle = (*txn2)->child(*root, "counter");
            REQUIRE(handle.has_value());
            
            auto value = (*txn2)->get_int(*handle);
            REQUIRE(value.has_value());
            REQUIRE(*value == 1);
            
            // Update the value
            auto set_result = (*txn2)->set_int(*handle, 2);
            REQUIRE(set_result.has_value());
            
            auto commit = (*txn2)->commit();
            REQUIRE(commit.has_value());
        }
        
        // Third transaction sees the updated value
        {
            auto txn3 = store->begin_transaction();
            REQUIRE(txn3.has_value());
            
            auto root = (*txn3)->root();
            REQUIRE(root.has_value());
            
            auto handle = (*txn3)->child(*root, "counter");
            REQUIRE(handle.has_value());
            
            auto value = (*txn3)->get_int(*handle);
            REQUIRE(value.has_value());
            REQUIRE(*value == 2);
        }
    }
    
    SECTION("Durability - Data persists across reopens") {
        // Create and commit data
        {
            auto txn = store->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            auto obj_result = (*txn)->make_object(*root, "config");
            REQUIRE(obj_result.has_value());
            
            auto str_result = (*txn)->make_string(*obj_result, "host", "localhost");
            REQUIRE(str_result.has_value());
            
            auto int_result = (*txn)->make_int(*obj_result, "port", 8080);
            REQUIRE(int_result.has_value());
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
        }
        
        // Close and reopen
        auto close_result = store->close();
        REQUIRE(close_result.has_value());
        
        auto new_store = make_toml_file_store(temp.path(), opts);
        REQUIRE(new_store.has_value());
        
        auto open = (*new_store)->open(temp.path());
        REQUIRE(open.has_value());
        
        // Verify data persisted
        {
            auto txn = (*new_store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            auto config = (*txn)->child(*root, "config");
            REQUIRE(config.has_value());
            
            auto host = (*txn)->child(*config, "host");
            REQUIRE(host.has_value());
            
            auto host_value = (*txn)->get_string(*host);
            REQUIRE(host_value.has_value());
            REQUIRE(*host_value == "localhost");
            
            auto port = (*txn)->child(*config, "port");
            REQUIRE(port.has_value());
            
            auto port_value = (*txn)->get_int(*port);
            REQUIRE(port_value.has_value());
            REQUIRE(*port_value == 8080);
        }
    }
}

TEST_CASE("TOML Transaction - Complex Data Structures", "[storage][toml]") {
    TempFile temp("test_complex.toml");
    TomlStoreOptions opts{};
    
    auto store = make_toml_file_store(temp.path(), opts);
    REQUIRE(store.has_value());
    
    auto open = (*store)->open(temp.path());
    REQUIRE(open.has_value());
    
    SECTION("Nested objects") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        // Create nested structure: { server: { database: { host: "localhost", port: 5432 } } }
        auto server = (*txn)->make_object(*root, "server");
        REQUIRE(server.has_value());
        
        auto database = (*txn)->make_object(*server, "database");
        REQUIRE(database.has_value());
        
        auto host_result = (*txn)->make_string(*database, "host", "localhost");
        REQUIRE(host_result.has_value());
        
        auto port_result = (*txn)->make_int(*database, "port", 5432);
        REQUIRE(port_result.has_value());
        
        // Navigate using the helper
        auto db_host = (*txn)->get<std::string>(*root, "server.database.host");
        REQUIRE(db_host.has_value());
        REQUIRE(*db_host == "localhost");
        
        auto db_port = (*txn)->get<int64_t>(*root, "server.database.port");
        REQUIRE(db_port.has_value());
        REQUIRE(*db_port == 5432);
        
        auto commit = (*txn)->commit();
        REQUIRE(commit.has_value());
    }
    
    SECTION("Arrays") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        // Create an array
        auto servers = (*txn)->make_array(*root, "servers");
        REQUIRE(servers.has_value());
        
        // Arrays in TOML can only contain values of the same type
        // Let's create an array of objects
        auto server1 = (*txn)->make_object(*servers, "");  // Empty key for array element
        REQUIRE_FALSE(server1.has_value());  // Arrays don't support named keys
        
        // Note: The current interface doesn't fully support array operations
        // This is a limitation that should be addressed
    }
    
    SECTION("All value types") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        // Test all supported types
        auto bool_result = (*txn)->make_bool(*root, "is_enabled", true);
        REQUIRE(bool_result.has_value());
        
        auto int_result = (*txn)->make_int(*root, "count", -42);
        REQUIRE(int_result.has_value());
        
        auto double_result = (*txn)->make_double(*root, "temperature", 98.6);
        REQUIRE(double_result.has_value());
        
        auto string_result = (*txn)->make_string(*root, "message", "Hello, TOML!");
        REQUIRE(string_result.has_value());
        
        // Verify values
        auto bool_handle = (*txn)->child(*root, "is_enabled");
        REQUIRE(bool_handle.has_value());
        auto bool_value = (*txn)->get_bool(*bool_handle);
        REQUIRE(bool_value.has_value());
        REQUIRE(*bool_value == true);
        
        auto int_handle = (*txn)->child(*root, "count");
        REQUIRE(int_handle.has_value());
        auto int_value = (*txn)->get_int(*int_handle);
        REQUIRE(int_value.has_value());
        REQUIRE(*int_value == -42);
        
        auto double_handle = (*txn)->child(*root, "temperature");
        REQUIRE(double_handle.has_value());
        auto double_value = (*txn)->get_double(*double_handle);
        REQUIRE(double_value.has_value());
        REQUIRE(*double_value == Catch::Approx(98.6));
        
        auto string_handle = (*txn)->child(*root, "message");
        REQUIRE(string_handle.has_value());
        auto string_value = (*txn)->get_string(*string_handle);
        REQUIRE(string_value.has_value());
        REQUIRE(*string_value == "Hello, TOML!");
    }
}

TEST_CASE("TOML Transaction - Error Handling", "[storage][toml]") {
    TempFile temp("test_errors.toml");
    TomlStoreOptions opts{};
    
    auto store = make_toml_file_store(temp.path(), opts);
    REQUIRE(store.has_value());
    
    auto open = (*store)->open(temp.path());
    REQUIRE(open.has_value());
    
    SECTION("Invalid key names") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        // Test invalid key patterns
        auto result1 = (*txn)->make_string(*root, "123invalid", "value");
        REQUIRE_FALSE(result1.has_value());
        REQUIRE(result1.error().code == ErrorCode::PathSyntax);
        
        auto result2 = (*txn)->make_string(*root, "has-dash", "value");
        REQUIRE_FALSE(result2.has_value());
        REQUIRE(result2.error().code == ErrorCode::PathSyntax);
        
        auto result3 = (*txn)->make_string(*root, "has space", "value");
        REQUIRE_FALSE(result3.has_value());
        REQUIRE(result3.error().code == ErrorCode::PathSyntax);
        
        // Valid keys
        auto result4 = (*txn)->make_string(*root, "valid_key", "value");
        REQUIRE(result4.has_value());
        
        auto result5 = (*txn)->make_string(*root, "_underscore", "value");
        REQUIRE(result5.has_value());
        
        auto result6 = (*txn)->make_string(*root, "CamelCase123", "value");
        REQUIRE(result6.has_value());
    }
    
    SECTION("Duplicate keys") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        auto result1 = (*txn)->make_string(*root, "duplicate", "first");
        REQUIRE(result1.has_value());
        
        auto result2 = (*txn)->make_string(*root, "duplicate", "second");
        REQUIRE_FALSE(result2.has_value());
        REQUIRE(result2.error().code == ErrorCode::AlreadyExists);
    }
    
    SECTION("Invalid handles") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        StoreHandle invalid{0};  // Invalid handle
        
        auto result = (*txn)->get_string(invalid);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == ErrorCode::InvalidArgument);
        
        StoreHandle nonexistent{999999};  // Valid format but doesn't exist
        
        auto result2 = (*txn)->get_string(nonexistent);
        REQUIRE_FALSE(result2.has_value());
        REQUIRE(result2.error().code == ErrorCode::KeyNotFound);
    }
    
    SECTION("Type mismatches on parent operations") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        // Create a string value
        auto str_result = (*txn)->make_string(*root, "not_a_table", "value");
        REQUIRE(str_result.has_value());
        
        auto str_handle = (*txn)->child(*root, "not_a_table");
        REQUIRE(str_handle.has_value());
        
        // Try to use it as a table
        auto child_result = (*txn)->make_string(*str_handle, "child", "value");
        REQUIRE_FALSE(child_result.has_value());
        REQUIRE(child_result.error().code == ErrorCode::TypeMismatch);
    }
}

TEST_CASE("TOML Transaction - Update Operations", "[storage][toml]") {
    TempFile temp("test_updates.toml");
    TomlStoreOptions opts{};
    
    auto store = make_toml_file_store(temp.path(), opts);
    REQUIRE(store.has_value());
    
    auto open = (*store)->open(temp.path());
    REQUIRE(open.has_value());
    
    SECTION("Update existing values") {
        // Create initial values
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            auto counter_result = (*txn)->make_int(*root, "counter", 1);
            REQUIRE(counter_result.has_value());
            
            auto status_result = (*txn)->make_string(*root, "status", "initial");
            REQUIRE(status_result.has_value());
            
            auto active_result = (*txn)->make_bool(*root, "active", false);
            REQUIRE(active_result.has_value());
            
            auto ratio_result = (*txn)->make_double(*root, "ratio", 0.5);
            REQUIRE(ratio_result.has_value());
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
        }
        
        // Update values
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            // Update each type
            auto counter = (*txn)->child(*root, "counter");
            REQUIRE(counter.has_value());
            auto set1 = (*txn)->set_int(*counter, 42);
            REQUIRE(set1.has_value());
            
            auto status = (*txn)->child(*root, "status");
            REQUIRE(status.has_value());
            auto set2 = (*txn)->set_string(*status, "updated");
            REQUIRE(set2.has_value());
            
            auto active = (*txn)->child(*root, "active");
            REQUIRE(active.has_value());
            auto set3 = (*txn)->set_bool(*active, true);
            REQUIRE(set3.has_value());
            
            auto ratio = (*txn)->child(*root, "ratio");
            REQUIRE(ratio.has_value());
            auto set4 = (*txn)->set_double(*ratio, 0.75);
            REQUIRE(set4.has_value());
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
        }
        
        // Verify updates
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            auto v1 = (*txn)->get<int64_t>(*root, "counter");
            REQUIRE(v1.has_value());
            REQUIRE(*v1 == 42);
            
            auto v2 = (*txn)->get<std::string>(*root, "status");
            REQUIRE(v2.has_value());
            REQUIRE(*v2 == "updated");
            
            auto v3 = (*txn)->get<bool>(*root, "active");
            REQUIRE(v3.has_value());
            REQUIRE(*v3 == true);
            
            auto v4 = (*txn)->get<double>(*root, "ratio");
            REQUIRE(v4.has_value());
            REQUIRE(*v4 == Catch::Approx(0.75));
        }
    }
    
    SECTION("Remove operations") {
        // Create structure
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            auto keep_result = (*txn)->make_string(*root, "keep_me", "value");
            REQUIRE(keep_result.has_value());
            
            auto remove_result = (*txn)->make_string(*root, "remove_me", "value");
            REQUIRE(remove_result.has_value());
            
            auto obj = (*txn)->make_object(*root, "nested");
            REQUIRE(obj.has_value());
            
            auto child1_result = (*txn)->make_string(*obj, "child1", "value1");
            REQUIRE(child1_result.has_value());
            
            auto child2_result = (*txn)->make_string(*obj, "child2", "value2");
            REQUIRE(child2_result.has_value());
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
        }
        
        // Remove items
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            // Remove a top-level key
            auto remove1 = (*txn)->remove(*root, "remove_me");
            REQUIRE(remove1.has_value());
            
            // Remove a nested key
            auto nested = (*txn)->child(*root, "nested");
            REQUIRE(nested.has_value());
            
            auto remove2 = (*txn)->remove(*nested, "child2");
            REQUIRE(remove2.has_value());
            
            // Try to remove non-existent key
            auto remove3 = (*txn)->remove(*root, "doesnt_exist");
            REQUIRE_FALSE(remove3.has_value());
            REQUIRE(remove3.error().code == ErrorCode::KeyNotFound);
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
        }
        
        // Verify removals
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            auto has1 = (*txn)->has(*root, "keep_me");
            REQUIRE(has1.has_value());
            REQUIRE(*has1 == true);
            
            auto has2 = (*txn)->has(*root, "remove_me");
            REQUIRE(has2.has_value());
            REQUIRE(*has2 == false);
            
            auto nested = (*txn)->child(*root, "nested");
            REQUIRE(nested.has_value());
            
            auto has3 = (*txn)->has(*nested, "child1");
            REQUIRE(has3.has_value());
            REQUIRE(*has3 == true);
            
            auto has4 = (*txn)->has(*nested, "child2");
            REQUIRE(has4.has_value());
            REQUIRE(*has4 == false);
        }
    }
}

TEST_CASE("TOML Store - File Format", "[storage][toml]") {
    TempFile temp("test_format.toml");
    TomlStoreOptions opts{};
    
    SECTION("Verify TOML output format") {
        auto store = make_toml_file_store(temp.path(), opts);
        REQUIRE(store.has_value());
        
        auto open = (*store)->open(temp.path());
        REQUIRE(open.has_value());
        
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            // Create a typical config structure
            auto title_result = (*txn)->make_string(*root, "title", "TOML Example");
            REQUIRE(title_result.has_value());
            
            auto owner = (*txn)->make_object(*root, "owner");
            REQUIRE(owner.has_value());
            auto owner_name_result = (*txn)->make_string(*owner, "name", "Tom Preston-Werner");
            REQUIRE(owner_name_result.has_value());
            
            auto database = (*txn)->make_object(*root, "database");
            REQUIRE(database.has_value());
            auto server_result = (*txn)->make_string(*database, "server", "192.168.1.1");
            REQUIRE(server_result.has_value());
            auto port_result = (*txn)->make_int(*database, "port", 5432);
            REQUIRE(port_result.has_value());
            auto enabled_result = (*txn)->make_bool(*database, "enabled", true);
            REQUIRE(enabled_result.has_value());
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
        }
        
        auto close_result = (*store)->close();
        REQUIRE(close_result.has_value());
        
        // Read the file and verify it's valid TOML
        std::string content = temp.read();
        REQUIRE_FALSE(content.empty());
        
        // Should contain our keys
        REQUIRE_THAT(content, ContainsSubstring("title"));
        REQUIRE_THAT(content, ContainsSubstring("TOML Example"));
        REQUIRE_THAT(content, ContainsSubstring("[owner]"));
        REQUIRE_THAT(content, ContainsSubstring("[database]"));
    }
    
    SECTION("Read existing TOML file") {
        // Write a TOML file manually
        temp.write(R"(
title = "Test Config"

[server]
host = "localhost"
port = 8080

[logging]
level = "debug"
file = "/var/log/app.log"
)");
        
        auto store = make_toml_file_store(temp.path(), opts);
        REQUIRE(store.has_value());
        
        auto open = (*store)->open(temp.path());
        REQUIRE(open.has_value());
        
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        // Verify we can read the existing data
        auto title = (*txn)->child(*root, "title");
        REQUIRE(title.has_value());
        auto title_value = (*txn)->get_string(*title);
        REQUIRE(title_value.has_value());
        REQUIRE(*title_value == "Test Config");
        
        auto server_port = (*txn)->get<int64_t>(*root, "server.port");
        REQUIRE(server_port.has_value());
        REQUIRE(*server_port == 8080);
        
        auto log_level = (*txn)->get<std::string>(*root, "logging.level");
        REQUIRE(log_level.has_value());
        REQUIRE(*log_level == "debug");
    }
}

TEST_CASE("TOML Store - Crash Safety", "[storage][toml]") {
    TempFile temp("test_crash.toml");
    TempFile temp_backup(temp.path().string() + ".tmp");
    TomlStoreOptions opts{};
    
    SECTION("Atomic file replacement") {
        // Create initial file
        temp.write(R"(
[initial]
value = "original"
)");
        
        auto store = make_toml_file_store(temp.path(), opts);
        REQUIRE(store.has_value());
        
        auto open = (*store)->open(temp.path());
        REQUIRE(open.has_value());
        
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            // Modify data
            auto result = (*txn)->make_string(*root, "new_key", "new_value");
            REQUIRE(result.has_value());
            
            // Before commit, the temporary file shouldn't exist
            REQUIRE_FALSE(temp_backup.exists());
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
            
            // After commit, temporary file should be cleaned up
            REQUIRE_FALSE(temp_backup.exists());
            
            // Original file should have new content
            std::string content = temp.read();
            REQUIRE_THAT(content, ContainsSubstring("new_key"));
            REQUIRE_THAT(content, ContainsSubstring("new_value"));
        }
    }
}

TEST_CASE("TOML Store - Performance", "[storage][toml][!benchmark]") {
    TempFile temp("test_perf.toml");
    TomlStoreOptions opts{};
    
    auto store = make_toml_file_store(temp.path(), opts);
    REQUIRE(store.has_value());
    
    auto open = (*store)->open(temp.path());
    REQUIRE(open.has_value());
    
    SECTION("Many keys") {
        auto start = std::chrono::high_resolution_clock::now();
        
        {
            auto txn = (*store)->begin_transaction();
            REQUIRE(txn.has_value());
            
            auto root = (*txn)->root();
            REQUIRE(root.has_value());
            
            // Create 1000 key-value pairs
            for (int i = 0; i < 1000; ++i) {
                std::string key = "key_" + std::to_string(i);
                std::string value = "value_" + std::to_string(i);
                
                auto result = (*txn)->make_string(*root, key, value);
                REQUIRE(result.has_value());
            }
            
            auto commit = (*txn)->commit();
            REQUIRE(commit.has_value());
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Should complete in reasonable time (adjust as needed)
        REQUIRE(duration.count() < 5000);  // 5 seconds
        
        // Verify file size is reasonable
        auto size = fs::file_size(temp.path());
        REQUIRE(size > 0);
        REQUIRE(size < 1'000'000);  // Less than 1MB for 1000 simple entries
    }
    
    SECTION("Deep nesting") {
        auto txn = (*store)->begin_transaction();
        REQUIRE(txn.has_value());
        
        auto root = (*txn)->root();
        REQUIRE(root.has_value());
        
        // Create deeply nested structure
        auto current = root;
        for (int i = 0; i < 10; ++i) {
            std::string key = "level_" + std::to_string(i);
            auto next = (*txn)->make_object(*current, key);
            REQUIRE(next.has_value());
            current = next;
        }
        
        // Add value at deepest level
        auto result = (*txn)->make_string(*current, "deep_value", "found_me");
        REQUIRE(result.has_value());
        
        auto commit = (*txn)->commit();
        REQUIRE(commit.has_value());
        
        // Should be able to navigate to deep value
        auto deep = (*txn)->navigate(*root, "level_0.level_1.level_2.level_3.level_4.level_5.level_6.level_7.level_8.level_9.deep_value");
        REQUIRE(deep.has_value());
        
        auto value = (*txn)->get_string(*deep);
        REQUIRE(value.has_value());
        REQUIRE(*value == "found_me");
    }
}