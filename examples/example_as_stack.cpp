#include "../streamer/generate.hpp"
#include "../streamer/stack.hpp"
#include <array>
#include <cassert>

/*
 * as_stack and as_stack() move the elements in the stream into a std::stack and returns the std::stack.
 * The first element of the stream is the last element to be popped.
 * as_stack and as_stack() force the steps of the stream to be executed.
 * as_stack and as_stack() cannot be used with an infinite stream.
*/
void example_as_stack() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::stack<int> result = input
        | as_stack;

    std::stack<int> result2 = input
        | as_stack();

    try {
        generator([]() { return 1; }) | as_stack;
        assert(false);
    } catch(const unbounded_stream &) {}


    std::deque<int> expected = {56, 3, 23, 100, 42};
    std::stack<int> expected_stack(expected);

    assert(result == expected_stack);
    assert(result2 == expected_stack);
    assert(result.top() == 42);
}
