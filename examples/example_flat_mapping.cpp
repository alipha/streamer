#include "../streamer/streamer.hpp"
#include "../streamer/vector.hpp"
#include <vector>
#include <cassert>
#include <string>

/*
 * mapping(MemOrFunc) returns a stream with the result of applying MemOrFunc to
 * each of the elements and then flattening those elements.
 *
 * MemOrFunc can be a unary function, or it can be a pointer-to-a-member-variable,
 * or it can be a pointer-to-a-member-function of type U (T::*p)() const.
 *
 * The result of MemOrFunc must be an iterable result. That is, it must have
 * a std::begin and a std::end. The result is then iterated and those elements are
 * appended to the resultant stream.
*/

struct person {
    person(std::string &&first, std::string &&last) : first_name(first), last_name(last) {}
    std::string first_name;
    std::string last_name;
};

struct family {
    family(std::vector<person> &&people) : members(people) {}

    std::vector<person> members;
};


bool operator==(const person &left, const person &right) {
    return left.last_name == right.last_name && left.first_name == right.first_name;
 }

bool operator==(const family &left, const family &right) {
    return left.members == right.members;
}


void example_flat_mapping() {
    using namespace streamer;

    std::vector<family> families = {
        family({
            person{"Bob", "Smith"},
            person{"Sue", "Johnson"},
            person{"Fred", "Smith-Johnson"}
        }),
        family({}),
        family({
            person{"Alice", "Parker"}
        })
    };
   
    std::vector<person> people = families
        | flat_mapping([](auto x) { return x.members; }) 
        | as_vector;

    std::vector<person> people2 = families
        % flat_mapping(&family::members)
        % as_vector;

    std::vector<std::string> names = families
        >> flat_mapping([](auto x) { 
            return x.members 
                | mapping([](auto x) { return x.first_name + " " + x.last_name; }); 
            })
        >> as_vector;


    std::vector<person> expected_people = {
        person{"Bob", "Smith"},
        person{"Sue", "Johnson"},
        person{"Fred", "Smith-Johnson"},
        person{"Alice", "Parker"}
    };

    std::vector<std::string> expected_names = {
        "Bob Smith",
        "Sue Johnson",
        "Fred Smith-Johnson",
        "Alice Parker"
    };

    assert(people == expected_people);
    assert(people2 == expected_people);
    assert(names == expected_names);
}
