#include "../streamer/streamer.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>

/*
 * filter(UnaryPred) returns a stream consisting of the elements for which UnaryPred(x) is true.
*/
void example_filter() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::vector<int> result = input
        | filter([](auto x) { return x > 50; }) 
        | as_vector;

    std::vector<int> expected = {56, 100};
    assert(result == expected);
}
