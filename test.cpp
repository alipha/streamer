#include "streamer.hpp"
#include <array>
#include <iostream>
#include <string>


int main() {
    std::array<std::string, 6> values{"45", "241", "8", "50", "2390", "3"};

    std::string test = streamer(values)
        .sort(&std::string::length, std::greater<int>())
        .join(", ");

    std::cout << test << std::endl;
 
    if(streamer(std::vector<int>()).first() 
            || streamer(values).first([](auto x) { return x == "9999"; }))
        std::cout << "Expected first to be empty" << std::endl;

    try {
        streamer(values).single();
        std::cout << "single() should throw" << std::endl;
    } catch(single_error e) {
        std::cout << e.what() << std::endl;
    }

    try {
        streamer(values).single([](auto x) { return x.length() == 1; });
        std::cout << "single(Func) should throw" << std::endl;
    } catch(single_error e) {
        std::cout << e.what() << std::endl;
    }

    std::string v = streamer(values)
        .first([](auto x) { return x.length() > 3; })
        .value();

    std::cout << "value: " << v << std::endl << std::endl;


    streamer_t<int> int_stream = streamer(values)
        .map([](auto x) { return std::stoi(x); });  // string to int

    streamer_t<int> int_stream2 = std::move(int_stream)
        .filter([](auto x) { return x > 10; });

    streamer_t<int> int_stream3;
    std::vector<int> result = std::move(int_stream2)
        .sort()
        .as_vector();

    for(auto &value : result)
        std::cout << value << std::endl;


    std::vector<std::pair<std::string, int>> name_ages{
        {"Alice", 30},
        {"Chris", 20},
        {"Bob", 25},
        {"Dave", 20},
        {"Eve", 20},
        {"Fred", 25}
    };

    streamer_t<std::string> names;
    names = streamer(name_ages)
        .map(&std::pair<std::string, int>::first);

    std::string name = std::move(names)
        .for_each([](auto x) { std::cout << x << std::endl; })
        .first()
        .value();

    std::cout << "first: " << name << std::endl;


    std::map<std::string, int> name_age_map = streamer(name_ages)
        .as_map(&std::pair<std::string, int>::first, &std::pair<std::string, int>::second);

    for(auto &item : name_age_map) {
        std::cout << "name: " << item.first << ", age: " << item.second << std::endl;
    }
    

    std::map<int, std::vector<std::string>> age_name_map = streamer(name_ages)
        .as_listmap(&std::pair<std::string, int>::second, &std::pair<std::string, int>::first);

    for(auto &names : age_name_map) {
        std::cout << names.first << ": " << streamer(names.second).join(", ") << std::endl;
    }

    
    std::map<unsigned int, std::vector<std::string>> len_value_map = streamer(values)
        .as_listmap(&std::string::length);

    std::string first_digits = streamer(len_value_map)
        .flat_map([](auto x) { 
            return streamer(x.second).map([](auto y) { return y[0]; });
        })
        .join(", ");

    std::cout << first_digits << std::endl;


    return 0;
}

