#include "../streamer/set.hpp"
#include <algorithm>
#include <array>
#include <cassert>

/*
 * as_set moves the elements in the stream into a std::set and returns the std::set.
 * as_set forces the steps of the stream to be executed.
 *
 * as_set(Comp) returns a std::set ordered by Comp.
 *
 * as_set throws a duplicate_set_key exception if multiple elements are considered
 * equal based upon the Comp (or std::less, if no Comp is specified.)
 *
 * as_set(false) and as_set(comp, false) may be used to ignore duplicate elements.
*/
void example_as_set() {
    using namespace streamer;

    std::array<int, 5> input = {56, 7, 23, 100, 42};
    std::array<int, 5> input2 = {20, 5, 4, 20, 3};
    
    std::set<int> result = input
        | as_set;

    std::set<int> result2 = input
        | as_set();

    std::set<int, std::greater<int> > result3 = input
        | as_set(std::greater<int>());

    auto result4 = input
        | as_set([](auto left, auto right) { return left % 10 < right % 10; });


    std::set<int> result5 = input2
        | as_set(false);

    std::set<int, std::greater<int> > result6 = input2
        | as_set(std::greater<int>(), false);


    try {
        input2 | as_set;
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input2 | as_set(std::greater<int>());
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input2 | as_set(true);
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input2 | as_set(std::greater<int>(), true);
        assert(false);
    } catch(const duplicate_set_key &) {}

   

    std::array<int, 5> expected = {7, 23, 42, 56, 100};
    auto expected2 = expected;
    std::array<int, 5> expected3 = {100, 56, 42, 23, 7};
    std::array<int, 5> expected4 = {100, 42, 23, 56, 7};
    std::array<int, 4> expected5 = {3, 4, 5, 20};
    std::array<int, 4> expected6 = {20, 5, 4, 3};

    assert(std::equal(result.begin(), result.end(), expected.begin(), expected.end()));
    assert(std::equal(result2.begin(), result2.end(), expected2.begin(), expected2.end()));
    assert(std::equal(result3.begin(), result3.end(), expected3.begin(), expected3.end()));
    assert(std::equal(result4.begin(), result4.end(), expected4.begin(), expected4.end()));
    assert(std::equal(result5.begin(), result5.end(), expected5.begin(), expected5.end()));
    assert(std::equal(result6.begin(), result6.end(), expected6.begin(), expected6.end()));
}
