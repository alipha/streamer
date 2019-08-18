#include "../streamer/vector.hpp"
#include <cassert>

/*
 * stream_empty<T>() creates an stream of type T containing no elements.
*/
void example_stream_empty() {
    using namespace streamer;
    
    std::vector<int> result = stream_empty<int>()
        | as_vector;

    std::vector<int> expected = {};
    assert(result == expected);
}
