#include "../streamer/streamer.hpp"
#include "../streamer/optional.hpp"
#include <cassert>
#include <vector>

/*
 * or_default(x) operates on a std::optional and returns either the value the
 * std::optional contains, or the value x passed to or_default if the std::optional is empty.
 * If no value is passed to or_default, then T() is returned if the std::optional is empty.
 */
void example_or_default() {
    using namespace streamer;

    std::vector<int> input = {15, 23, 4};
    std::vector<int> empty_input = {};

    int result = input >> first >> or_default(999);
    int result2 = empty_input >> first >> or_default(999);
    int result3 = empty_input >> first >> or_default();
    int result4 = empty_input >> first >> or_default;
    int result5 = input >> first >> or_default;

    assert(result == 15);
    assert(result2 == 999);
    assert(result3 == 0);
    assert(result4 == 0);
    assert(result5 == 15);
}
