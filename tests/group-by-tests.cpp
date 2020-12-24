#include <Lz/GroupBy.hpp>
#include <catch2/catch.hpp>
#include <list>


TEST_CASE("GroupBy changing and creating elements", "[GroupBy][Basic functionality]") {
	std::vector<std::string> vec = {
		"hello", "hellp", "i'm", "done"
	};

	auto grouper = lz::groupBy(vec, [](const std::string& s) { return s.length(); });

	SECTION("Should be correct chunks") {
		std::size_t stringLength = 3;

		for (auto&& g : grouper) {
			CHECK(g.first == stringLength);

			for (auto& str : g.second) {
				CHECK(str.length() == stringLength);
			}

			++stringLength;
		}
	}

	SECTION("Should be by ref") {
		auto begin = grouper.begin();
		*(begin->second.begin()) = "imm";
		CHECK(vec[0] == "imm");
	}

	SECTION("Should be sorted") {
		CHECK(std::is_sorted(vec.begin(), vec.end(), [](const std::string& l, const std::string& r) {
			return l.length() < r.length();
		}));
	}

	SECTION("Should be still be sorted descending after groupBy") {
		std::sort(vec.begin(), vec.end(), [](const std::string& l, const std::string& r) {
			return l.length() > r.length();
		});
#ifdef LZ_HAS_EXECUTION
		auto g = lz::groupBy(vec, [](const std::string& s) { return s.length(); }, std::execution::seq, false);
#else
		auto g = lz::groupBy(vec, [](const std::string& s) { return s.length(); }, false);
#endif
		static_cast<void>(g);
		CHECK(std::is_sorted(vec.begin(), vec.end(), [](const std::string& l, const std::string& r) {
			return l.length() > r.length();
		}));
	}
}

TEST_CASE("GroupBy binary operations", "[GroupBy][Binary ops]") {
	std::vector<std::string> vec = {
		"hello", "hellp", "i'm", "done"
	};
	auto grouper = lz::groupBy(vec, [](const std::string& s) { return s.length(); });

	SECTION("Operator++") {
		auto it = grouper.begin();
		CHECK(it->first == 3);
		CHECK(*it->second.begin() == "i'm");
		++it;

		CHECK(it->first == 4);
		CHECK(*it->second.begin() == "done");
	}

	SECTION("Operator== & operator!=") {
		auto it = grouper.begin();
		CHECK(it != grouper.end());
		it = grouper.end();
		CHECK(it == grouper.end());
	}
}