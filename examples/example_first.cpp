#include "../streamer/streamer.hpp"
#include <cassert>
#include <vector>

/*
 * first and first() return the first element in the stream, or an empty std::optional if
 * there are no elements in the stream.
 *
 * first(UnaryPred) returns the first element in the stream matching the predicate, or 
 * an empty std::optional if no elements match the predicate.
*/
void example_first() {
    using namespace streamer;

    std::vector<int> input = {8, 3, 23, 100, 4};
    std::vector<int> empty_input = {};
    
    std::optional<int> result = input 
        >> first([](auto x) { return x > 10; });

    std::optional<int> no_result = input
        >> first([](auto x) { return x > 1000; });

    std::optional<int> result2 = input >> first;
    std::optional<int> no_result2 = empty_input >> first();

    assert(result.value() == 23);
    assert(!no_result);
    assert(result2.value() == 8);
    assert(!no_result2);
}
