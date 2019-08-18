#include "../streamer/generate.hpp"
#include "../streamer/order.hpp"
#include "../streamer/vector.hpp"
#include <cassert>
#include <string>

/*
 * generator(GenFunc) creates an infinite stream by repeatedly calling the no-argument GenFunc()
 * and using the return values as the stream elements.
 *
 * generator(seed, UnaryFunc) creates an infinite stream by first passing in the seed to UnaryFunc,
 * then passing in the result of UnaryFunc to the next call of UnaryFunc.
 * The elements would be: seed, UnaryFunc(seed), UnaryFunc(UnaryFunc(seed)), ...
 * The type of stream generated is the type of the UnaryFunc result, not the type of seed.
*/
void example_generator() {
    using namespace streamer;
    
    int counter = 0;
    std::vector<int> result = generator([&counter]() { ++counter; return counter * counter; })
        | take_while([](auto x) { return x < 50; })
        | as_vector;

    std::vector<int> result2 = generator(5, [](auto x) { return x + x; })
        % take(5)
        % as_vector();

    std::vector<std::string> result3 = generator("abc", [](std::string x) { return x + x; })
        >> take(3)
        >> as_vector;


    std::vector<int> expected = {1, 4, 9, 16, 25, 36, 49};
    std::vector<int> expected2 = {5, 10, 20, 40, 80};
    std::vector<std::string> expected3 = {"abc", "abcabc", "abcabcabcabc"};

    assert(result == expected);
    assert(result2 == expected2);
    assert(result3 == expected3);
}
