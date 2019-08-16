#include "../streamer/order.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * take_while(UnaryPred) include elements in the stream until UnaryPred is false.
 * If UnaryPred is false for the first element, then the resulting stream is empty.
 * If UnaryPred is true for all elements, then the whole stream is returned.
*/
void example_take_while() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::vector<int> result = input
        | take_while([](auto x) { return x < 80; })
        | as_vector;

    std::vector<int> result2 = input
        | take_while([](auto x) { return x > 80; })
        | as_vector;

    std::vector<int> result3 = input
        | take_while([](auto x) { return x < 1000; })
        | as_vector;

    std::vector<int> expected = {56, 3, 23};
    std::vector<int> expected2 = {};
    std::vector<int> expected3 = {56, 3, 23, 100, 42};

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
}
