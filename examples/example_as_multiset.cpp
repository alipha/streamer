#include "../streamer/generate.hpp"
#include "../streamer/set.hpp"
#include <algorithm>
#include <array>
#include <cassert>


void example_as_multiset_basic();
void example_as_multiset_member_func();
void example_as_multiset_member_var();

/*
 * as_multiset moves the elements in the stream into a std::multiset and returns the std::multiset.
 * as_multiset forces the steps of the stream to be executed.
 *
 * as_multiset(CompOrMem) returns a std::multiset ordered by CompOrMem.
 * CompOrMem can be a normal comparison functor, or it can be a
 * member function pointer or a pointer to a member variable.
 * That member's default comparer is then used for comparison, or a Comp
 * functor can be specified as below.
 *
 * as_multiset(Mem, Comp) returns a std::multiset ordered by the Comp applied
 * to the specified member function or member variable. 
 *
 * as_multiset cannot be used with an infinite stream.
*/
void example_as_multiset() {
    example_as_multiset_basic();
    example_as_multiset_member_func();
    example_as_multiset_member_var();
}


void example_as_multiset_basic() {
    using namespace streamer;

    std::array<int, 5> input = {56, 7, 23, 100, 23};

    
    std::multiset<int> result = input
        | as_multiset;

    std::multiset<int> result2 = input
        | as_multiset();

    std::multiset<int, std::greater<int> > result3 = input
        | as_multiset(std::greater<int>());

    auto result4 = input
        | as_multiset([](auto left, auto right) { return left % 10 < right % 10; });


    try {
        generator([]() { return 1; }) | as_multiset;
        assert(false);
    } catch(const unbounded_stream &) {}

    try {
        generator([]() { return 1; }) | as_multiset(std::greater<int>());
        assert(false);
    } catch(const unbounded_stream &) {}


    std::array<int, 5> expected = {7, 23, 23, 56, 100};
    auto expected2 = expected;
    std::array<int, 5> expected3 = {100, 56, 23, 23, 7};
    std::array<int, 5> expected4 = {100, 23, 23, 56, 7};

    assert(std::equal(result.begin(), result.end(), expected.begin(), expected.end()));
    assert(std::equal(result2.begin(), result2.end(), expected2.begin(), expected2.end()));
    assert(std::equal(result3.begin(), result3.end(), expected3.begin(), expected3.end()));
    assert(std::equal(result4.begin(), result4.end(), expected4.begin(), expected4.end()));
}


void example_as_multiset_member_func() {
    using namespace streamer;

    std::array<std::string, 5> input2 = {"hello", "hey", "hello", "hola", "hi"};

    auto result5 = input2
        | as_multiset(&std::string::length);

    auto result6 = input2
        | as_multiset(&std::string::length, std::greater());

    std::array<std::string, 5> expected5 = {"hi", "hey", "hola", "hello", "hello"};
    std::array<std::string, 5> expected6 = {"hello", "hello", "hola", "hey", "hi"};

    assert(std::equal(result5.begin(), result5.end(), expected5.begin(), expected5.end()));
    assert(std::equal(result6.begin(), result6.end(), expected6.begin(), expected6.end()));
}


void example_as_multiset_member_var() {
    using namespace streamer;
    using sipair = std::pair<std::string, int>;

    std::array<sipair, 3> input3 = {
        sipair{"Apple", 10},
        sipair{"Banana", 3},
        sipair{"Carrot", 5}
    };

    auto result7 = input3
        | as_multiset(&sipair::second);

    auto result8 = input3
        | as_multiset(&sipair::second, std::greater());

    std::array<sipair, 3> expected7 = {
        sipair{"Banana", 3},
        sipair{"Carrot", 5},
        sipair{"Apple", 10}
    };
    std::array<sipair, 3> expected8 = {
        sipair{"Apple", 10},
        sipair{"Carrot", 5},
        sipair{"Banana", 3}
    };

    assert(std::equal(result7.begin(), result7.end(), expected7.begin(), expected7.end()));
    assert(std::equal(result8.begin(), result8.end(), expected8.begin(), expected8.end()));

}

