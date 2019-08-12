#include "../streamer/queue.hpp"
#include <array>
#include <cassert>

/*
 * as_queue and as_queue() move the elements in the stream into a std::queue and returns the std::queue.
 * The first element of the stream is the first element to be popped.
 * as_queue and as_queue() force the steps of the stream to be executed.
*/
void example_as_queue() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::queue<int> result = input
        | as_queue;

    std::queue<int> result2 = input
        | as_queue();

    std::deque<int> expected = {56, 3, 23, 100, 42};
    std::queue<int> expected_queue(expected);

    assert(result == expected_queue);
    assert(result2 == expected_queue);
    assert(result.front() == 56);
}
