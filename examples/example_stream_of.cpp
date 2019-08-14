#include "../streamer/streamer.hpp"
#include "../streamer/vector.hpp"
#include <cassert>

/*
 * stream_of(...) creates a new stream by moving the values in the initialization list into the stream.
*/
void example_stream_of() {
    using namespace streamer;

    std::vector<int> result = stream_of({56, 3, 23, 100, 42})
        | as_vector;

    std::vector<int> expected = {56, 3, 23, 100, 42};

    assert(result == expected);
}
