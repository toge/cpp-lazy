#include "Lz/CartesianProduct.hpp"
#include <catch2/catch.hpp>
#include <list>

TEST_CASE("Cartesian product changing and creating elements", "[CartesianProduct][Basic functionality]") {
    std::vector<int> vec = {1, 2, 3};
    std::vector<char> chars = {'a', 'b', 'c'};

    auto cartesian = lz::cartesian(vec, chars);

    SECTION("Should be by reference") {
        auto& elmVec = std::get<0>(*cartesian.begin());
        auto& elmChars = std::get<1>(*cartesian.begin());

        CHECK(&elmVec == &vec[0]);
        CHECK(&elmChars == &chars[0]);
    }

    SECTION("Should make combinations") {
        CHECK(*cartesian.begin() == std::make_tuple(1, 'a'));
        CHECK(*++cartesian.begin() == std::make_tuple(1, 'b'));
    }

    SECTION("Should be correct length") {
        CHECK(std::distance(cartesian.begin(), cartesian.end()) == static_cast<std::ptrdiff_t>(vec.size() * chars.size()));
    }
}

TEST_CASE("Cartesian product binary operations", "[CartesianProduct][Binary ops]") {
    std::vector<int> vec = {1, 2, 3};
    std::vector<char> chars = {'a', 'b', 'c'};
    auto cartesian = lz::cartesian(vec, chars);

    SECTION("Operator++") {
    	auto begin = cartesian.begin();
        ++begin;
        CHECK(*begin == std::make_tuple(1, 'b'));
        ++begin, ++begin;
        CHECK(*begin == std::make_tuple(2, 'a'));
    }

    SECTION("Operator== & operator!=") {
        auto it = cartesian.begin();
        CHECK(it != cartesian.end());
        it = cartesian.end();
        CHECK(it == cartesian.end());
    }
}

TEST_CASE("CartesianProduct to containers", "[CartesianProduct][To container]") {
    std::vector<int> vec = {1,2,3};
    std::vector<char> chars = {'a', 'b', 'c'};
    auto cartesian = lz::cartesian(vec, chars);

    SECTION("To array") {
        constexpr std::size_t size = 9;
        std::array<std::tuple<int, char>, size> result = cartesian.toArray<size>();
        std::array<std::tuple<int, char>, size> expected = {
            std::make_tuple(1, 'a'),
            std::make_tuple(1, 'b'),
            std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'),
            std::make_tuple(2, 'b'),
            std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'),
            std::make_tuple(3, 'b'),
            std::make_tuple(3, 'c'),
        };
        CHECK(result == expected);
    }

    SECTION("To vector") {
        std::vector<std::tuple<int, char>> result = cartesian.toVector();
        std::vector<std::tuple<int, char>> expected = {
            std::make_tuple(1, 'a'),
            std::make_tuple(1, 'b'),
            std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'),
            std::make_tuple(2, 'b'),
            std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'),
            std::make_tuple(3, 'b'),
            std::make_tuple(3, 'c'),
        };
        CHECK(result == expected);
    }

    SECTION("To other container using to<>()") {
        std::list<std::tuple<int, char>> result = cartesian.to<std::list>();
        std::list<std::tuple<int, char>> expected = {
            std::make_tuple(1, 'a'),
            std::make_tuple(1, 'b'),
            std::make_tuple(1, 'c'),

            std::make_tuple(2, 'a'),
            std::make_tuple(2, 'b'),
            std::make_tuple(2, 'c'),

            std::make_tuple(3, 'a'),
            std::make_tuple(3, 'b'),
            std::make_tuple(3, 'c'),
        };
        CHECK(result == expected);
    }

    SECTION("To map") {
        std::map<int, std::tuple<int, char>> result = cartesian.toMap([](const typename decltype(cartesian)::iterator::value_type& v) {
            return std::get<0>(v) + std::get<1>(v);
        });
        decltype(result) expected = {
            {1 + 'a', std::make_tuple(1, 'a')},
            {1 + 'b', std::make_tuple(1, 'b')},
            {1 + 'c', std::make_tuple(1, 'c')},

            {2 + 'a', std::make_tuple(2, 'a')},
            {2 + 'b', std::make_tuple(2, 'b')},
            {2 + 'c', std::make_tuple(2, 'c')},

            {3 + 'a', std::make_tuple(3, 'a')},
            {3 + 'b', std::make_tuple(3, 'b')},
            {3 + 'c', std::make_tuple(3, 'c')}
        };
        CHECK(result == expected);
    }

    SECTION("To unordered map") {
        std::unordered_map<int, std::tuple<int, char>> result =
                cartesian.toUnorderedMap([](const typename decltype(cartesian)::iterator::value_type& v) {
            return std::get<0>(v) + std::get<1>(v);
        });
        decltype(result) expected = {
            {1 + 'a', std::make_tuple(1, 'a')},
            {1 + 'b', std::make_tuple(1, 'b')},
            {1 + 'c', std::make_tuple(1, 'c')},

            {2 + 'a', std::make_tuple(2, 'a')},
            {2 + 'b', std::make_tuple(2, 'b')},
            {2 + 'c', std::make_tuple(2, 'c')},

            {3 + 'a', std::make_tuple(3, 'a')},
            {3 + 'b', std::make_tuple(3, 'b')},
            {3 + 'c', std::make_tuple(3, 'c')}
        };
        CHECK(result == expected);
    }
}