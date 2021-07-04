#include "Lz/ChunkIf.hpp"

#include <catch2/catch.hpp>
#include <list>

TEST_CASE("ChunkIf changing and creating elements", "[ChunkIf][Basic functionality]") {
    std::string s = "hello world; this is a message;";
    auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });

    SECTION("Length should be correct") {
        CHECK(std::distance(chunked.begin(), chunked.end()) == 2);
    }

    SECTION("Should be by reference") {
        auto it = chunked.begin();
        *it->begin() = 'w';
        CHECK(s[0] == 'w');
    }

    SECTION("Should chunk correct length") {
        auto it = chunked.begin();
        CHECK(std::distance(it->begin(), it->end()) == static_cast<std::ptrdiff_t>(std::string("hello world").length()));
        ++it;
        CHECK(std::distance(it->begin(), it->end()) == static_cast<std::ptrdiff_t>(std::string(" this is a message").length()));
    }
}

TEST_CASE("ChunkIf binary operations", "[ChunkIf][Binary ops]") {
    std::string s = "hello world; this is a message;";
    auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
    CHECK(chunked.begin()->toString() == "hello world");

    SECTION("Operator++") {
        auto it = chunked.begin();
        ++it;
        CHECK(it->toString() == " this is a message");
        ++it;
        CHECK(it == chunked.end());
    }

    SECTION("Operator== & operator!=") {
        auto begin = chunked.begin();
        CHECK(begin != chunked.end());
        begin = chunked.end();
        CHECK(begin == chunked.end());
    }
}

TEST_CASE("ChunkIf to containers", "[ChunkIf][To container]") {
    std::string s = "hello world; this is a message";
    auto chunked = lz::chunkIf(s, [](const char c) { return c == ';'; });
    using Iterator = decltype(*chunked.begin());

    SECTION("To array") {
        std::array<std::string, 2> arr;
        std::transform(chunked.begin(), chunked.end(), arr.begin(), [](const Iterator& it) { return it.toString(); });
        CHECK(arr == decltype(arr){ "hello world", " this is a message" });
    }

    SECTION("To vector") {
        std::vector<std::string> vec;
        std::transform(chunked.begin(), chunked.end(), std::back_inserter(vec), [](const Iterator& it) { return it.toString(); });
        CHECK(vec == decltype(vec){ "hello world", " this is a message" });
    }

    SECTION("To other container using to<>()") {
        std::list<std::string> list;
        std::transform(chunked.begin(), chunked.end(), std::inserter(list, list.begin()),
                       [](const Iterator& it) { return it.toString(); });
        CHECK(list == decltype(list){ "hello world", " this is a message" });
    }
}