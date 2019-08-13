#include "../streamer/order.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * take(n) limits the stream to n elements. If the stream has more than n elements,
 * only the first n elements are returned.
*/
void example_take() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::vector<int> result = input
        | take(3)
        | as_vector;

    std::vector<int> result2 = input
        | take(0)
        | as_vector;

    std::vector<int> expected = {56, 3, 23};
    std::vector<int> expected2 = {};

    assert(result == expected);
    assert(result2 == expected2);
}
