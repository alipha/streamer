#include "../streamer/streamer.hpp"
#include <cassert>
#include <vector>

/*
 * last and last() return the last element in the stream, or an empty std::optional if
 * there are no elements in the stream.
 *
 * last(UnaryPred) returns the last element in the stream matching the predicate, or 
 * an empty std::optional if no elements match the predicate.
*/
void example_last() {
    using namespace streamer;

    std::vector<int> input = {8, 3, 23, 100, 4};
    std::vector<int> empty_input = {};
    
    std::optional<int> result = input 
        >> last([](auto x) { return x > 10; });

    std::optional<int> no_result = input
        >> last([](auto x) { return x > 1000; });

    std::optional<int> result2 = input >> last;
    std::optional<int> no_result2 = empty_input >> last();

    assert(result.value() == 100);
    assert(!no_result);
    assert(result2.value() == 4);
    assert(!no_result2);
}
