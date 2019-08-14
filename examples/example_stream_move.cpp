#include "../streamer/streamer.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>
#include <string>
#include <vector>

/*
 * stream_move(Cont&) creates a new stream by referring to the container's elements.
 * Elements are moved from the container into the stream when the stream is processed.
 * The container must exist for the duration of the stream's life.
 *
 * stream_move(const T (&)[N]) creates a new stream which refers to the array's elements.
 * Elements are moved from the array into the stream when the stream is processed.
 * The array must exist for the duration of the stream's life.
 *
 * stream_move(const T *p, n) creates a new stream which refers to the array of length n pointed at by p.
 * Elements are moved from the array into the stream when the stream is processed.
 * The array must exist for the duration of the stream's life.
 *
 * stream_move(It, It) creates a new stream which refers to the elements contained within the iterator range.
 * Elements are moved from the iterator range into the stream when the stream is processed.
 * The iterators must remain valid for the duration of the stream's life.
 *
*/
void example_stream_move() {
    using namespace streamer;

    std::array<std::string, 5> input = {"56", "3", "23", "100", "42"};
    std::vector<std::string> input2 = {"10", "20", "30", "40", "50", "60", "70"};
    std::string input3[] = {"2", "3", "5", "7", "11", "13"};
    std::string input4[] = {"123", "456"};
    std::string *ptr = input4;

    
    std::vector<std::string> result = stream_move(input) 
        | as_vector;

    std::vector<std::string> result2 = stream_move(input2.begin() + 1, input2.end() - 2)
        | as_vector();

    std::vector<std::string> result3 = stream_move(input3)
        >> as_vector;

    std::vector<std::string> result4 = stream_move(ptr, 2)
        | as_vector;


    std::vector<std::string> expected = {"56", "3", "23", "100", "42"};
    std::vector<std::string> expected2 = {"20", "30", "40", "50"};
    std::vector<std::string> expected3 = {"2", "3", "5", "7", "11", "13"};
    std::vector<std::string> expected4 = {"123", "456"};

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
    assert(result4 == expected4);
}
