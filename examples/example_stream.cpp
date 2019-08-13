#include "../streamer/streamer.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>
#include <vector>

/*
 * stream(Cont&&) creates a new stream by moving the container's elements into the stream.
 * This function is called implicitly with the use of the stream operators ( >>, |, and % )
 *
 * stream(Cont&) creates a new stream by referring to the container's elements.
 * The container must exist for the duration of the stream's life.
 * This function is called implicitly with the use of the stream operators.
 *
 * stream(streamer_t&&) is a no-op. (It constructs a new streamer using the streamer's move constructor).
 *
 * stream(const T (&)[N]) creates a new stream which refers to the array's elements.
 * The array must exist for the duration of the stream's life.
 * This function is called implicitly with the use of the stream operators.
 *
 * stream(const T *p, n) creates a new stream which refers to the array of length n pointed at by p.
 * The array must exist for the duration of the stream's life.
 *
 * stream(It, It) creates a new stream which refers to the elements contained within the iterator range.
 * The iterators must remain valid for the duration of the stream's life.
 *
*/
void example_stream() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    std::vector<int> input2 = {10, 20, 30};
    int input3[] = {2, 3, 5, 7, 11, 13};

    
    std::vector<int> result = stream(input) 
        | as_vector;

    std::vector<int> result2 = stream(input.begin() + 1, input.end() - 2)
        | as_vector();

    std::vector<int> result3 = stream(std::move(input))     // most efficient
        >> as_vector;

    std::vector<int> result4 = stream(input3)
        | as_vector;


    // calling stream() is unnecessary, except for result2

    std::vector<int> result5 = input2 
        % as_vector();

    std::vector<int> result6 = std::move(input2)    // most efficient
        % as_vector;

    std::vector<int> result7 = input3
        | as_vector;


    std::vector<int> expected = {56, 3, 23, 100, 42};
    std::vector<int> expected2 = {3, 23};
    auto expected3 = expected;
    std::vector<int> expected4 = {2, 3, 5, 7, 11, 13};

    std::vector<int> expected5 = {10, 20, 30};
    auto expected6 = expected5;
    auto expected7 = expected4;

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
    assert(result4 == expected4);
    assert(result5 == expected5);
    assert(result6 == expected6);
    assert(result7 == expected7);
}
