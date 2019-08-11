#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * as_vector and as_vector() move the elements in the stream into a std::vector and returns the std::vector.
 * as_vector and as_vector() force the steps of the stream to be executed.
*/
void example_as_vector() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::vector<int> result = input
        | as_vector;

    std::vector<int> result2 = input
        | as_vector();

    std::vector<int> expected = {56, 3, 23, 100, 42};
    assert(result == expected);
    assert(result2 == expected);
}
