#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/catch_approx.hpp>
#include <ion/core/store.h>
#include <filesystem>
#include <fstream>

using namespace ion::core;
using namespace Catch::Matchers;
namespace fs = std::filesystem;

class temp_file {
    fs::path path_;
public:
    temp_file(const std::string& name) 
        : path_(fs::temp_directory_path() / name) {
        cleanup();
    }
    
    ~temp_file() { cleanup(); }
    
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

TEST_CASE("JSON Store - Basic Operations", "[storage][json]") {
    temp_file temp("test_basic.json");
    json_store_options opts{};
    
    SECTION("Create and open empty store") {
        auto store_result = make_json_file_store(temp.path(), opts);
        REQUIRE(store_result.has_value());
        auto& store = *store_result;
        
        auto open_result = store->open(temp.path());
        REQUIRE(open_result.has_value());
        
        auto close_result = store->close();
        REQUIRE(close_result.has_value());
    }
    
    SECTION("Open non-existent file creates it") {
        REQUIRE_FALSE(temp.exists());
        
        auto store_result = make_json_file_store(temp.path(), opts);
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
        auto store_result = make_json_file_store(temp.path(), opts);
        REQUIRE(store_result.has_value());
        auto& store = *store_result;
        
        auto open1 = store->open(temp.path());
        REQUIRE(open1.has_value());
        
        auto open2 = store->open(temp.path());
        REQUIRE_FALSE(open2.has_value());
        REQUIRE(open2.error() == core_errc::already_exists);
    }
    
    SECTION("Cannot begin transaction on closed store") {
        auto store_result = make_json_file_store(temp.path(), opts);
        REQUIRE(store_result.has_value());
        auto& store = *store_result;
        
        // Don't open the store
        auto txn_result = store->begin_transaction();
        REQUIRE_FALSE(txn_result.has_value());
        REQUIRE(txn_result.error() == core_errc::invalid_state);
    }
}

TEST_CASE("JSON Transaction - ACID Properties", "[storage][json][acid]") {
    temp_file temp("test_acid.json");
    json_store_options opts{};
    
    auto store_result = make_json_file_store(temp.path(), opts);
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
            
            // Add more data but rollback
            auto result3 = txn->make_string(*root, "key3", "value3");
            REQUIRE(result3.has_value());
            
            // Don't commit - should rollback
        }
        
        // Third transaction - verify original data is still there
        {
            auto txn_result = store->begin_transaction();
            REQUIRE(txn_result.has_value());
            auto& txn = *txn_result;
            
            auto root = txn->root();
            REQUIRE(root.has_value());
            
            auto has_key1 = txn->has(*root, "key1");
            REQUIRE(has_key1.has_value());
            REQUIRE(*has_key1);
            
            auto has_key2 = txn->has(*root, "key2");
            REQUIRE(has_key2.has_value());
            REQUIRE(*has_key2);
            
            auto has_key3 = txn->has(*root, "key3");
            REQUIRE(has_key3.has_value());
            REQUIRE_FALSE(*has_key3); // This should not exist
        }
    }
}

TEST_CASE("JSON Transaction - Data Types", "[storage][json][types]") {
    temp_file temp("test_types.json");
    json_store_options opts{};
    
    auto store_result = make_json_file_store(temp.path(), opts);
    REQUIRE(store_result.has_value());
    auto& store = *store_result;
    
    auto open_result = store->open(temp.path());
    REQUIRE(open_result.has_value());
    
    auto txn_result = store->begin_transaction();
    REQUIRE(txn_result.has_value());
    auto& txn = *txn_result;
    
    auto root = txn->root();
    REQUIRE(root.has_value());
    
    SECTION("Boolean operations") {
        auto make_result = txn->make_bool(*root, "test_bool", true);
        REQUIRE(make_result.has_value());
        
        auto child_result = txn->child(*root, "test_bool");
        REQUIRE(child_result.has_value());
        
        auto get_result = txn->get_bool(*child_result);
        REQUIRE(get_result.has_value());
        REQUIRE(*get_result == true);
        
        auto set_result = txn->set_bool(*child_result, false);
        REQUIRE(set_result.has_value());
        
        auto get_result2 = txn->get_bool(*child_result);
        REQUIRE(get_result2.has_value());
        REQUIRE(*get_result2 == false);
    }
    
    SECTION("Integer operations") {
        auto make_result = txn->make_int(*root, "test_int", 42);
        REQUIRE(make_result.has_value());
        
        auto child_result = txn->child(*root, "test_int");
        REQUIRE(child_result.has_value());
        
        auto get_result = txn->get_int(*child_result);
        REQUIRE(get_result.has_value());
        REQUIRE(*get_result == 42);
    }
    
    SECTION("Double operations") {
        auto make_result = txn->make_double(*root, "test_double", 3.14159);
        REQUIRE(make_result.has_value());
        
        auto child_result = txn->child(*root, "test_double");
        REQUIRE(child_result.has_value());
        
        auto get_result = txn->get_double(*child_result);
        REQUIRE(get_result.has_value());
        REQUIRE(*get_result == Catch::Approx(3.14159));
    }
    
    SECTION("String operations") {
        auto make_result = txn->make_string(*root, "test_string", "Hello, World!");
        REQUIRE(make_result.has_value());
        
        auto child_result = txn->child(*root, "test_string");
        REQUIRE(child_result.has_value());
        
        auto get_result = txn->get_string(*child_result);
        REQUIRE(get_result.has_value());
        REQUIRE(*get_result == "Hello, World!");
    }
    
    SECTION("Array operations") {
        auto array_result = txn->make_array(*root, "test_array");
        REQUIRE(array_result.has_value());
        
        // Arrays work differently - they hold elements by index
        auto has_element = txn->has_element(*array_result, 0);
        REQUIRE(has_element.has_value());
        REQUIRE_FALSE(*has_element);
    }
    
    SECTION("Object operations") {
        auto object_result = txn->make_object(*root, "test_object");
        REQUIRE(object_result.has_value());
        
        auto make_nested = txn->make_string(*object_result, "nested_key", "nested_value");
        REQUIRE(make_nested.has_value());
        
        auto has_nested = txn->has(*object_result, "nested_key");
        REQUIRE(has_nested.has_value());
        REQUIRE(*has_nested);
    }
}