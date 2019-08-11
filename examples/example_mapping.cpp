#include "../streamer/streamer.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>
#include <string>

/*
 * mapping(MemOrFunc) returns a stream with the result of applying MemOrFunc to
 * each of the elements.
 *
 * MemOrFunc can be a unary function, or it can be a pointer-to-a-member-variable,
 * or it can be a pointer-to-a-member-function of type U (T::*p)() const.
*/
void example_mapping() {
    using namespace streamer;

    std::vector<std::string> input = {"56", "3", "2334", "100", "42"};
   
    std::vector<int> result = input
        | mapping([](auto x) { return std::stoi(x); }) 
        | as_vector;

    std::vector<std::string::size_type> result2 = input
        >> mapping(&std::string::length)
        >> as_vector;

    std::vector<int> result3 = input
        | mapping([](auto x) { return std::make_pair(x, std::stoi(x)); })
        | mapping(&std::pair<std::string, int>::second)
        | as_vector;


    std::vector<int> expected = {56, 3, 2334, 100, 42};

    std::vector<std::string::size_type> expected2 = {2, 1, 4, 3, 2};

    auto expected3 = expected;

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
}
