#include "../streamer/deque.hpp"
#include <array>
#include <cassert>

/*
 * as_deque and as_deque() move the elements in the stream into a std::deque and returns the std::deque.
 * as_deque and as_deque() force the steps of the stream to be executed.
*/
void example_as_deque() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::deque<int> result = input
        | as_deque;

    std::deque<int> result2 = input
        | as_deque();

    std::deque<int> expected = {56, 3, 23, 100, 42};
    assert(result == expected);
    assert(result2 == expected);
}
