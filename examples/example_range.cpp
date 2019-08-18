#include "../streamer/generate.hpp"
#include "../streamer/vector.hpp"
#include <array>
#include <cassert>
#include <string>
#include <iostream>


void example_range_basic();
void example_range_func();
void example_range_comp();

/*
 * range(min, max) creates a stream with the elements by incrementing from min to max, 
 * but not including max.
 * If max is less than or equal to min, then an empty stream is created. 
 * 
 * range(min, max, step) creates a stream by starting with the element min. step is
 * repeatedly added (using +=) to create the remaining elements in the stream as long as
 * the new element is less than max and greater than min.
 *
 * range(min, max, Unaryfunc) creates a stream by starting with the element min.
 * Unaryfunc is applied to the latest element to produce a new element until an
 * element is produced outside of the range [min, max).
 *
 * range(min, max, step, comp) is the same as range(min, max, step), except comp
 * is used to test if an element is within [min, max) instead of std::less.
 *
 * range(min, max, UnaryFunc, comp) is the same as range(min, max, Unary), except comp
 * is used to test if an element is within [min, max) instead of std::less.
*/
void example_range() {
    example_range_basic();
    example_range_func();
    example_range_comp();
}


void example_range_basic() {
    using namespace streamer;

    std::vector<int> result = range(15, 20)
        | as_vector;

    std::vector<int> result2 = range(100, 100)
        >> as_vector();

    std::vector<int> result3 = range(50, 40)
        % as_vector;

    std::vector<int> result4 = range(30, 48, 5)
        | as_vector();

    std::vector<int> result5 = range(30, 50, 5)
        | as_vector;

    std::vector<int> result6 = range(30, 50, -5)
        | as_vector;

    std::vector<int> result7 = range(50, 30, -5)
        | as_vector;

    using namespace std::string_literals;
    std::vector<std::string> result8 = range("foo"s, "foobarbarbar"s, "bar"s)
        | as_vector;


    std::vector<int> expected = {15, 16, 17, 18, 19};
    std::vector<int> expected2 = {};
    std::vector<int> expected3 = {};
    std::vector<int> expected4 = {30, 35, 40, 45};
    std::vector<int> expected5 = {30, 35, 40, 45};
    std::vector<int> expected6 = {30};
    std::vector<int> expected7 = {};
    std::vector<std::string> expected8 = {"foo", "foobar", "foobarbar"};

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
    assert(result4 == expected4);
    assert(result5 == expected5);
    assert(result6 == expected6);
    assert(result7 == expected7);
    assert(result8 == expected8);
}


void example_range_func() {
    using namespace streamer;

    std::vector<int> result = range(1, 50, [](auto x) { return x + x; })
        | as_vector;

    std::vector<int> result2 = range(1, 32, [](auto x) { return x + x; })
        | as_vector;

    std::vector<int> result3 = range(10, 10, [](auto x) { return x + x; })
        | as_vector;

    std::vector<int> result4 = range(10, 20, [](auto x) { return x / 2; })
        | as_vector;

    std::vector<int> result5 = range(10, 5, [](auto x) { return x / 2; })
        | as_vector;

    using namespace std::string_literals;
    std::vector<std::string> result6 = range("foobar"s, "z"s,
            [](auto x) { return x.substr(1) + x.front(); })
        | as_vector;


    std::vector<int> expected = {1, 2, 4, 8, 16, 32};
    std::vector<int> expected2 = {1, 2, 4, 8, 16};
    std::vector<int> expected3 = {};
    std::vector<int> expected4 = {10};
    std::vector<int> expected5 = {};
    std::vector<std::string> expected6 = {"foobar", "oobarf", "obarfo"};

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
    assert(result4 == expected4);
    assert(result5 == expected5);
    assert(result6 == expected6);
}


void example_range_comp() {
    using namespace streamer;

    
}

