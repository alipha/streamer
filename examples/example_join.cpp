#include "../streamer/generate.hpp"
#include "../streamer/join.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * join(delim) will return a string containing the elements of the stream
 * joined together by delim. The elements are << into a basic_stringstream.
 * 
 * If delim is a const char* or std::string, a std::string is returned.
 * If delim is a const wchar_t* or std::wstring, a std::wstring is returned.
*/
void example_join() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    std::array<double, 1> input2 = {4.58};
    std::vector<int> input3 = {};

    
    std::string result = input | join(", ");
    std::string result2 = input2 | join(", ");
    std::string result3 = input3 | join(", ");
    std::wstring result4 = input | join(L":");
    std::string result5 = input | join("");

    try {
        generator([]() { return 1; }) | join("x");
        assert(false);
    } catch(const unbounded_stream &) {}


    std::string expected = "56, 3, 23, 100, 42";
    std::string expected2 = "4.58";
    std::string expected3 = "";
    std::wstring expected4 = L"56:3:23:100:42";
    std::string expected5 = "5632310042";

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
    assert(result4 == expected4);
    assert(result5 == expected5);
}
