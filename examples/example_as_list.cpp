#include "../streamer/generate.hpp"
#include "../streamer/list.hpp"
#include <array>
#include <cassert>

/*
 * as_list and as_list() move the elements in the stream into a std::list and returns the std::list.
 * as_list and as_list() force the steps of the stream to be executed.
 * as_list and as_list() cannot be used with an infinite stream.
 */
void example_as_list() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::list<int> result = input
        | as_list;

    std::list<int> result2 = input
        | as_list();

    try {
        generator([]() { return 1; }) | as_list;
        assert(false);
    } catch(const unbounded_stream &) {}


    std::list<int> expected = {56, 3, 23, 100, 42};
    assert(result == expected);
    assert(result2 == expected);
}
