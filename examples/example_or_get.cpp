#include "../streamer/streamer.hpp"
#include "../streamer/optional.hpp"
#include <cassert>
#include <vector>

/*
 * or_get(UnaryFunc) operates on a std::optional and returns either the value the
 * std::optional contains, or calls the UnaryFunc passed to or_default if the std::optional is
 * empty, and returns the UnaryFunc's result.
 */
void example_or_get() {
    using namespace streamer;

    std::vector<int> input = {15, 23, 4};
    std::vector<int> empty_input = {};

    int count = 0;
    int count2 = 0;
    int result = input 
        >> first 
        >> or_get([&count]() { count++; return 999; });

    int result2 = input 
        % first 
        % or_get([&count]() { count++; return 999; });

    int result3 = empty_input 
        | first()
        | or_get([&count2]() { count2++; return 999; });

    assert(result == 15);
    assert(result2 == 15);
    assert(result3 == 999);
    assert(count == 0);
    assert(count2 == 1);
}
