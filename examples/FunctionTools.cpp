#include "Lz/FunctionTools.hpp"

int main() {
    std::vector<int> ints = {1, 2, 3, 4};

    double avg = lz::mean(ints); // avg == (1. + 2. + 3. + 4.) / 4.)
    double median = lz::median(ints); // median == (2 + 3) / 2.)

    std::vector<std::string> strings = {"hello", "world", "what's", "up"};
    auto unlines = lz::unlines(strings).toString(); // unlines == "hello\nworld\nwhat's\nup"

    std::string string = "aa\nbb\nbb";
    auto lines = lz::lines(string).toVector(); // lines == std::vector<std::string>{"aa", "bb", "bb"}

    std::vector<std::string> s = {"hello", "world", "!"};
	
#if (!defined(LZ_HAS_CXX_17)) && (!defined(LZ_HAS_CXX_20))
    size_t totalSize = lz::transAccumulate(s, 0U, [](size_t i, const std::string& s) {
        return i + s.size();
    }); // totalSize == 11
#endif

    std::string toFind = "hel";
    std::string def = "default";

    toFind = std::move(lz::firstOrDefault(s, toFind, def));
    // toFind == "default"

    toFind = "hello";
    toFind = std::move(lz::firstOrDefault(s, toFind, def));
    // toFind == "hello"

    toFind = lz::firstOrDefaultIf(s, [](const std::string& s) {
        return s.find('\'') != std::string::npos; // ' is present in the sequence
    }, def);
    // toFind == "what's"

    toFind = lz::firstOrDefaultIf(s, [](const std::string& s) {
        return s.find('z') != std::string::npos; // z is not present in the sequence
    }, "default");
    // toFind == "default"

    std::vector<char> c = {'a', 'b', 'c', 'd'};
    size_t pos = lz::indexOf(c, 'b');
    // pos == 1

    pos = lz::indexOf(c, 'e');
    // pos == lz::npos

    strings = {"hello", "world"};
    pos = lz::indexOfIf(strings, [](const std::string& s) {
        return s.find('o') != std::string::npos;
    });
    // pos == 0

    pos = lz::indexOfIf(strings, [](const std::string& s) {
        return s.find('q') != std::string::npos;
    });
    // pos == lz::npos


    std::string str = "123,d35dd";
    auto f = lz::filterMap(str, [](const char c) { return static_cast<bool>(std::isdigit(c)); }, // if this is true
                           [](const char c) { return static_cast<int>(c - '0'); }); // return this
    // f will yield {1, 2, 3, 3, 5}


    std::string myString = "picture.jpg";
    lz::strReplace(myString, ".jpg", ".png");
    // myString == "picture.png"

    myString = "picture.png.png";
    lz::strReplaceAll(myString, ".png", ".jpg");
    // myString == "picture.jpg.jpg"

    auto arr = { 1, 2, 3, 4 };
	for (auto&& vals : lz::pairwise(arr)) {
		// printing values yields (using std::get):
		// 1 2
		// 2 3
		// 3 4
	}

    myString = "picture.png.png";
    myString = lz::reverse(myString).toString();
	// myString == gnp.gnp.erutcip

    int summed = lz::sumTo(50000); // 1 + 2 + 3 + 4 + 5 + ... 50'000
	// summed == 1250025000

    s = {"hello world", "what's up"};
    toFind = "hel";
    def = "default";

    toFind = lz::lastOrDefault(s, toFind, def);
    // toFind == "default"

    toFind = "what's up";
    // toFind == "what's up"

    toFind = lz::lastOrDefaultIf(s, [](const std::string& s) {
        return s.find('\'') != std::string::npos;
    }, def);
    // toFind == "what's up"

    toFind = lz::lastOrDefaultIf(s, [](const std::string& s) {
        return lz::contains(s, 'q');
    }, def);
    // toFind == def
}