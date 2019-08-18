#include "../streamer/streamer.hpp"
#include "../streamer/optional.hpp"
#include <cassert>
#include <vector>

/*
 * or_throw operates on a std::optional and returns either the value the
 * std::optional contains, or throws an exception if the std::optional is empty.
 *
 * If or_throw has no argument, then std::bad_optional_access is thrown if the
 * std::optional is empty.
 *
 * If an object derived from std::exception is passed to or_throw, then that
 * exception is thrown if std::optional is empty.
 *
 * Else, the argument to or_throw is assumed to be a functor, and the object that
 * is returned from the functor is thrown.
 */
void example_or_throw() {
    using namespace streamer;

    std::vector<int> input = {15, 23, 4};
    std::vector<int> empty_input = {};

    int result = input >> first >> or_throw;
    int result2 = input >> first >> or_throw();

    int result3 = input 
        % first 
        % or_throw(std::runtime_error("shouldn't get thrown"));

    int count = 0;
    int result4 = input 
        | first 
        | or_throw([&count]() { ++count; return std::runtime_error("also shouldn't get thrown"); });
    
    try {
        empty_input >> first >> or_throw;
        assert(false);
    } catch(std::bad_optional_access &) {}

    try {
        empty_input >> first >> or_throw();
        assert(false);
    } catch(std::bad_optional_access &) {}

    try {
        empty_input >> first >> or_throw(std::runtime_error("thrown!"));
        assert(false);
    } catch(std::runtime_error &) {}

    int count2 = 0;
    try {
        empty_input 
            >> first 
            >> or_throw([&count2] { ++count2; return std::runtime_error("also thrown!"); });
        assert(false);
    } catch(std::runtime_error &) {}

    assert(result == 15);
    assert(result2 == 15);
    assert(result3 == 15);
    assert(result4 == 15);
    assert(count == 0);
    assert(count2 == 1);
}
