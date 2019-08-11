#include "../streamer/streamer.hpp"
#include <cassert>
#include <vector>

/*
 * single and single() return the element in the stream, or an empty std::optional if
 * there are no elements in the stream. If there is more than one element in the stream,
 * a single_error is thrown.
 *
 * single(UnaryPred) returns the element in the stream matching the predicate, or 
 * an empty std::optional if no elements match the predicate. If more than one element
 * matches the predicate, a single_error is thrown.
*/
void example_single() {
    using namespace streamer;

    std::vector<int> input = {8, 3, 23, 100, 4};
    std::vector<int> one_input {55};
    std::vector<int> empty_input = {};
    
    std::optional<int> result = input 
        >> single([](auto x) { return x > 50; });

    std::optional<int> no_result = input
        >> single([](auto x) { return x > 1000; });

    std::optional<int> result2 = one_input >> single;
    std::optional<int> no_result2 = empty_input >> single();

    try {
        input >> single();
        assert(false);
    } catch(single_error &) {}

    try {
        input >> single([](auto x) { return x > 10; });
        assert(false);
    } catch(single_error &) {}

    assert(result.value() == 100);
    assert(!no_result);
    assert(result2.value() == 55);
    assert(!no_result2);
}
