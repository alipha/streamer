#include "../streamer/streamer.hpp"
#include <array>
#include <cassert>

/*
 * item_count and item_count() return a count of how many elements are in the stream.
 * item_count(UnaryPred) returns a count of how many elements match UnaryPred.
 * item_count, item_count() and item_count(UnaryPred) force execution of the stream.
*/
void example_item_count() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::size_t result = input >> item_count;
    std::size_t result2 = input >> item_count();
    std::size_t result3 = input % item_count([](auto x) { return x > 50; }); 

    assert(result == 5);
    assert(result2 == 5);
    assert(result3 = 2);
}
