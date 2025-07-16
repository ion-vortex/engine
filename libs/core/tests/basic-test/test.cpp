#include <catch2/catch_test_macros.hpp>
#include <oxide/core/buffer.h>
#include <string_view>
#include <memory>

TEST_CASE("VectorBuffer basic test", "[vector_buffer]") {
    using namespace oxide::core;

    SECTION("Create buffer") {
        auto buffer = create_buffer(1024);
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->size() == 0);
        REQUIRE(buf->capacity() >= 1024);
    }

    SECTION("Reserve space") {
        auto buffer = create_buffer(1024);
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->reserve(512).has_value());
        REQUIRE(buf->size() == 0);
    }

    SECTION("Append data") {
        using namespace std::literals::string_view_literals;
        auto buffer = create_buffer(1024);
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->append(std::as_bytes(std::span{"Hello, World!"sv})).has_value());
        REQUIRE(buf->size() == 13);
    }
}