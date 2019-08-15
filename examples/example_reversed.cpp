#include "../streamer/order.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * reversed and reversed() reverse the order of the elements in the stream.
 * reversed and reversed() force the steps of the stream to be executed.
*/
void example_reversed() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::vector<int> result = input
        | reversed
        | as_vector;

    std::vector<int> result2 = input
        | reversed()
        | as_vector;

    std::vector<int> expected = {42, 100, 23, 3, 56};
    assert(result == expected);
    assert(result2 == expected);
}
