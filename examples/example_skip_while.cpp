#include "../streamer/order.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * skip_while(UnaryPred) skips elements in the stream until UnaryPred is false.
 * If UnaryPred is true for all elements, then the resulting stream is empty.
*/
void example_skip_while() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::vector<int> result = input
        | skip_while([](auto x) { return x < 80; })
        | as_vector;

    std::vector<int> result2 = input
        | skip_while([](auto x) { return x > 80; })
        | as_vector;

    std::vector<int> result3 = input
        | skip_while([](auto x) { return x < 1000; })
        | as_vector;

    std::vector<int> expected = {100, 42};
    std::vector<int> expected2 = {56, 3, 23, 100, 42};
    std::vector<int> expected3 = {};

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
}
