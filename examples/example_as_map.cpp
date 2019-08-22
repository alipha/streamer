#include "../streamer/generate.hpp"
#include "../streamer/map.hpp"
#include <algorithm>
#include <array>
#include <cassert>

void example_as_map_basic();
//void example_as_map_member_func();
//void example_as_map_member_var();

/*
 * as_map moves the elements in the stream into a std::map and returns the std::map.
 * as_map forces the steps of the stream to be executed.
 *
 * as_map(CompOrMem) returns a std::map ordered by CompOrMem.
 * CompOrMem can be a normal comparison functor, or it can be a
 * member function pointer or a pointer to a member variable.
 * That member's default comparer is then used for comparison, or a Comp
 * functor can be specified as below.
 *
 * as_map(Mem, Comp) returns a std::map ordered by the Comp applied
 * to the specified member function or member variable.
 *
 * as_map throws a duplicate_map_key exception if multiple elements are considered
 * equal based upon the Comp (or std::less, if no Comp is specified.)
 *
 * as_map(false), as_map(comp, false), or as_map(&mem, comp, false) may be used to
 * ignore duplicate elements.
 *
 * as_map cannot be used with an infinite stream.
*/
void example_as_map() {
    example_as_map_basic();
    //example_as_map_member_func();
    //example_as_map_member_var();
}
#include <iostream>

void example_as_map_basic() {
    using namespace streamer;

    std::array<std::string, 4> input = {"hey", "hello", "hola", "hi"};
    std::array<std::string, 5> input2 = {"howdy", "hey", "hello", "hola", "hi"};

    auto result = input
        | as_map(&std::string::length);

    auto result2 = input
        | as_map(&std::string::length, identity<std::string>(), std::greater());

    auto result3 = input2
        | as_map(&std::string::length, false);

    auto result4 = input2
        | as_map(&std::string::length, identity<std::string>(), std::greater(), false);

    auto result5 = input2
        | as_map(&std::string::length, [](const auto &x) { return x.back(); }, false);

    try {
        input2 | as_map(&std::string::length);
        assert(false);
    } catch(const duplicate_map_key &) {}

    try {
        input2 | as_map(&std::string::length, identity<std::string>(), std::greater());
        assert(false);
    } catch(const duplicate_map_key &) {}

    try {
        input2 | as_map(&std::string::length, true);
        assert(false);
    } catch(const duplicate_map_key &) {}

    try {
        input2 | as_map(&std::string::length, identity<std::string>(), std::greater(), true);
        assert(false);
    } catch(const duplicate_map_key &) {}


    using p = std::pair<const std::string::size_type, std::string>;
    std::array<p, 4> expected =  {p{2, "hi"},    p{3, "hey"},  p{4, "hola"}, p{5, "hello"}};
    std::array<p, 4> expected2 = {p{5, "hello"}, p{4, "hola"}, p{3, "hey"},  p{2, "hi"}};
    std::array<p, 4> expected3 = {p{2, "hi"},    p{3, "hey"},  p{4, "hola"}, p{5, "howdy"}};
    std::array<p, 4> expected4 = {p{5, "howdy"}, p{4, "hola"}, p{3, "hey"},  p{2, "hi"}};

    using p2 = std::pair<const std::string::size_type, char>;
    std::array<p2, 4> expected5 = {p2{2, 'i'},   p2{3, 'y'},   p2{4, 'a'},   p2{5, 'y'}};

    assert(std::equal(result.begin(), result.end(), expected.begin(), expected.end()));
    assert(std::equal(result2.begin(), result2.end(), expected2.begin(), expected2.end()));
    assert(std::equal(result3.begin(), result3.end(), expected3.begin(), expected3.end()));
    assert(std::equal(result4.begin(), result4.end(), expected4.begin(), expected4.end()));
    assert(std::equal(result5.begin(), result5.end(), expected5.begin(), expected5.end()));
}

/*
void example_as_map_member_var() {
    using namespace streamer;
    using sipair = std::pair<std::string, int>;

    std::array<sipair, 3> input5 = {
        sipair{"Apple", 10},
        sipair{"Banana", 3},
        sipair{"Carrot", 5}
    };
    
    std::array<sipair, 4> input6 = {
        sipair{"Apple", 10},
        sipair{"Banana", 3},
        sipair{"Carrot", 5},
        sipair{"Date", 3}
    };


    auto result11 = input5
        | as_map(&sipair::second);

    auto result12 = input5
        | as_map(&sipair::second, std::greater());

    auto result13 = input6
        | as_map(&sipair::second, false);

    auto result14 = input6
        | as_map(&sipair::second, std::greater(), false);

    try {
        input6 | as_map(&sipair::second);
        assert(false);
    } catch(const duplicate_map_key &) {}

    try {
        input6 | as_map(&sipair::second, std::greater());
        assert(false);
    } catch(const duplicate_map_key &) {}

    try {
        input6 | as_map(&sipair::second, true);
        assert(false);
    } catch(const duplicate_map_key &) {}

    try {
        input6 | as_map(&sipair::second, std::greater(), true);
        assert(false);
    } catch(const duplicate_map_key &) {}


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
*/
