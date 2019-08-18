#include "../streamer/generate.hpp"
#include "../streamer/deque.hpp"
#include <array>
#include <cassert>

/*
 * as_deque and as_deque() move the elements in the stream into a std::deque and returns the std::deque.
 * as_deque and as_deque() force the steps of the stream to be executed.
 * as_deque and as_deque() cannot be used with an infinite stream.
*/
void example_as_deque() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::deque<int> result = input
        | as_deque;

    std::deque<int> result2 = input
        | as_deque();

    try {
        generator([]() { return 1; }) | as_deque;
        assert(false);
    } catch(const unbounded_stream &) {}


    std::deque<int> expected = {56, 3, 23, 100, 42};
    assert(result == expected);
    assert(result2 == expected);
}
