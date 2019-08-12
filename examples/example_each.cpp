#include "../streamer/streamer.hpp"
#include <array>
#include <cassert>
#include <vector>

/*
 * each(UnaryPred) calls UnaryPred with each element of the stream.
 * each(UnaryPred) the steps of the stream to be executed.
 * each(UnaryPred) has no return value.
*/
void example_each() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    std::vector<int> result;

    input >> each([&result](auto x) { result.push_back(x * 10); });

    std::vector<int> expected = {560, 30, 230, 1000, 420};
    assert(result == expected);
}
