#include "../streamer/streamer.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>


void example_exclude() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    //std::vector<int> result =
    std::move(input)
        >> exclude([](auto x) { return x > 50; }); 
//        >> as_vector;

    std::vector<int> expected = {3, 23, 42};
    //assert(result == expected);
}
