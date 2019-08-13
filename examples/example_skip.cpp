#include "../streamer/order.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * skip(n) skips the first n elements in the stream. If the stream has fewer than n elements,
 * then the resulting stream is now empty.
*/
void example_skip() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::vector<int> result = input
        | skip(3)
        | as_vector;

    std::vector<int> result2 = input
        | skip(0)
        | as_vector;

    std::vector<int> expected = {100, 42};
    std::vector<int> expected2 = {56, 3, 23, 100, 42};
    assert(result == expected);
    assert(result2 == expected2);
}
