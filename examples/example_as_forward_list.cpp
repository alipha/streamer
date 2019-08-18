#include "../streamer/generate.hpp"
#include "../streamer/forward_list.hpp"
#include <array>
#include <cassert>

/*
 * as_forward_list and as_forward_list() move the elements in the stream into
 * std::forward_list and returns the std::forward_list.
 * as_forward_list and as_forward_list() force the steps of the stream to be executed.
 * as_forward_list and as_forward_list() cannot be used with an infinite stream.
*/
void example_as_forward_list() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::forward_list<int> result = input
        | as_forward_list;

    std::forward_list<int> result2 = input
        | as_forward_list();

    try {
        generator([]() { return 1; }) | as_forward_list;
        assert(false);
    } catch(const unbounded_stream &) {}


    std::forward_list<int> expected = {56, 3, 23, 100, 42};
    assert(result == expected);
    assert(result2 == expected);
}
