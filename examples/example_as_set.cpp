#include "../streamer/set.hpp"
#include <algorithm>
#include <array>
#include <cassert>

void example_as_set_basic();
void example_as_set_member_func();
void example_as_set_member_var();

/*
 * as_set moves the elements in the stream into a std::set and returns the std::set.
 * as_set forces the steps of the stream to be executed.
 *
 * as_set(CompOrMem) returns a std::set ordered by CompOrMem.
 * CompOrMem can be a normal comparison functor, or it can be a
 * member function pointer or a pointer to a member variable.
 * That member's default comparer is then used for comparison, or a Comp
 * functor can be specified as below.
 *
 * as_set(Mem, Comp) returns a std::set ordered by the Comp applied
 * to the specified member function or member variable.
 *
 * as_set throws a duplicate_set_key exception if multiple elements are considered
 * equal based upon the Comp (or std::less, if no Comp is specified.)
 *
 * as_set(false), as_set(comp, false), or as_set(&mem, comp, false) may be used to
 * ignore duplicate elements.
*/
void example_as_set() {
    example_as_set_basic();
    example_as_set_member_func();
    example_as_set_member_var();
}


void example_as_set_basic() {
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


void example_as_set_member_func() {
    using namespace streamer;

    std::array<std::string, 4> input3 = {"hey", "hello", "hola", "hi"};
    std::array<std::string, 5> input4 = {"howdy", "hey", "hello", "hola", "hi"};


    auto result7 = input3
        | as_set(&std::string::length);

    auto result8 = input3
        | as_set(&std::string::length, std::greater());

    auto result9 = input4
        | as_set(&std::string::length, false);

    auto result10 = input4
        | as_set(&std::string::length, std::greater(), false);

    try {
        input4 | as_set(&std::string::length);
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input4 | as_set(&std::string::length, std::greater());
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input4 | as_set(&std::string::length, true);
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input4 | as_set(&std::string::length, std::greater(), true);
        assert(false);
    } catch(const duplicate_set_key &) {}


    std::array<std::string, 4> expected7 = {"hi", "hey", "hola", "hello"};
    std::array<std::string, 4> expected8 = {"hello", "hola", "hey", "hi"};
    std::array<std::string, 4> expected9 = {"hi", "hey", "hola", "howdy"};
    std::array<std::string, 4> expected10 = {"howdy", "hola", "hey", "hi"};
    
    assert(std::equal(result7.begin(), result7.end(), expected7.begin(), expected7.end()));
    assert(std::equal(result8.begin(), result8.end(), expected8.begin(), expected8.end()));
    assert(std::equal(result9.begin(), result9.end(), expected9.begin(), expected9.end()));
    assert(std::equal(result10.begin(), result10.end(), expected10.begin(), expected10.end()));
}


void example_as_set_member_var() {
    using namespace streamer;
    using sipair = std::pair<std::string, int>;

    std::array<sipair, 3> input5 = {
        sipair{"Apple", 10},
        sipair{"Banana", 3},
        sipair{"Carrot", 5}
    };
    
    std::array<sipair, 3> input6 = {
        sipair{"Apple", 10},
        sipair{"Banana", 3},
        sipair{"Carrot", 5},
        sipair{"Date", 3}
    };


    auto result11 = input5
        | as_set(&sipair::second);

    auto result12 = input5
        | as_set(&sipair::second, std::greater());

    auto result13 = input6
        | as_set(&sipair::second, false);

    auto result14 = input6
        | as_set(&sipair::second, std::greater(), false);

    try {
        input6 | as_set(&sipair::second);
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input6 | as_set(&sipair::second, std::greater());
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input6 | as_set(&sipair::second, true);
        assert(false);
    } catch(const duplicate_set_key &) {}

    try {
        input6 | as_set(&sipair::second, std::greater(), true);
        assert(false);
    } catch(const duplicate_set_key &) {}


    std::array<sipair, 3> expected11 = {
        sipair{"Banana", 3},
        sipair{"Carrot", 5},
        sipair{"Apple", 10}
    };
    std::array<sipair, 3> expected12 = {
        sipair{"Apple", 10},
        sipair{"Carrot", 5},
        sipair{"Banana", 3}
    };
    auto expected13 = expected11;
    auto expected14 = expected12;

    assert(std::equal(result11.begin(), result11.end(), expected11.begin(), expected11.end()));
    assert(std::equal(result12.begin(), result12.end(), expected12.begin(), expected12.end()));
    assert(std::equal(result13.begin(), result13.end(), expected13.begin(), expected13.end()));
    assert(std::equal(result14.begin(), result14.end(), expected14.begin(), expected14.end()));
}

