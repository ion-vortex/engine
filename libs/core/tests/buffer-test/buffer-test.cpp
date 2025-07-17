#include <catch2/catch_test_macros.hpp>
#include <oxide/core/buffer.h>
#include <string_view>
#include <memory>

TEST_CASE("Buffer - VectorBuffer", "[vector_buffer]") {
    using namespace std::literals::string_view_literals;
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
        auto buffer = create_buffer(1024);
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->capacity() >= 1024);
        REQUIRE(buf->size() == 0);
        REQUIRE(buf->append(std::as_bytes(std::span{"Hello, World!"sv})).has_value());
        REQUIRE(buf->size() == 13);
        REQUIRE(buf->view().size() == 13);
        REQUIRE(std::string_view(reinterpret_cast<const char*>(buf->view().data()), buf->view().size()) == "Hello, World!");
    }

    SECTION("Resize buffer") {
        auto buffer = create_buffer(1024);
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->resize(512).has_value());
        REQUIRE(buf->size() == 512);
        REQUIRE(buf->capacity() >= 512);
    }

    SECTION("Clear buffer") {
        auto buffer = create_buffer(1024);
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->append(std::as_bytes(std::span{"Test data"sv})).has_value());
        REQUIRE(buf->size() > 0);
        REQUIRE(buf->clear().has_value());
        REQUIRE(buf->size() == 0);
    }

    SECTION("Shrink to fit") {
        auto buffer = create_buffer(1024);
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->append(std::as_bytes(std::span{"Data to shrink"sv})).has_value());
        REQUIRE(buf->size() > 0);
        REQUIRE(buf->shrink_to_fit().has_value());
        REQUIRE(buf->capacity() >= buf->size());
    }    
}

TEST_CASE("Buffer - StaticBuffer", "[static_buffer]") {
    using namespace std::literals::string_view_literals;
    using namespace oxide::core;

    SECTION("Create static buffer") {
        auto buffer = create_static_buffer<256>();
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->size() == 0);
        REQUIRE(buf->capacity() == 256);
    }

    SECTION("Append to static buffer") {
        auto buffer = create_static_buffer<256>();
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->append(std::as_bytes(std::span{"Static data"sv})).has_value());
        REQUIRE(buf->size() == 11);
    }

    SECTION("Resize static buffer") {
        auto buffer = create_static_buffer<256>();
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->resize(128).has_value());
        REQUIRE(buf->size() == 128);
    }

    SECTION("Clear static buffer") {
        auto buffer = create_static_buffer<256>();
        REQUIRE(buffer.has_value());
        auto & buf = *buffer;
        REQUIRE(buf->append(std::as_bytes(std::span{"Clear me"sv})).has_value());
        REQUIRE(buf->size() > 0);
        REQUIRE(buf->clear().has_value());
        REQUIRE(buf->size() == 0);
    }
}