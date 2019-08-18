#include "../streamer/generate.hpp"
#include "../streamer/streamer.hpp"
#include <array>
#include <cassert>

/*
 * item_count and item_count() return a count of how many elements are in the stream.
 * item_count(UnaryPred) returns a count of how many elements match UnaryPred.
 * item_count, item_count() and item_count(UnaryPred) force execution of the stream.
 * item_count, item_count() and item_count(UnaryPred) cannot be used with an infinite stream.
*/
void example_item_count() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    std::size_t result = input >> item_count;
    std::size_t result2 = input >> item_count();
    std::size_t result3 = input % item_count([](auto x) { return x > 50; }); 


    try {
        generator([]() { return 1; }) | item_count;
        assert(false);
    } catch(const unbounded_stream &) {}

    try {
        generator([]() { return 1; }) | item_count([](auto x) { return x > 50; });
        assert(false);
    } catch(const unbounded_stream &) {}

    assert(result == 5);
    assert(result2 == 5);
    assert(result3 = 2);
}
