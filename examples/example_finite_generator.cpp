#include "../streamer/generate.hpp"
#include "../streamer/order.hpp"
#include "../streamer/vector.hpp"
#include <cassert>
#include <string>

/*
 * finite_generator(GenFunc) creates a stream by repeatedly calling the no-argument GenFunc()
 * and using the return values as the stream elements.
 *
 * GenFunc must return a std::optional. An empty optional indicates the end of the stream.
 *
 *
 * finite_generator(seed, UnaryFunc) creates an stream by first passing in the seed to UnaryFunc,
 * then passing in the result of UnaryFunc to the next call of UnaryFunc.
 *
 * The elements would be: seed, UnaryFunc(seed), UnaryFunc(UnaryFunc(seed)), ...
 *
 * seed must be of type std::optional and UnaryFunc must accept a std::optional and return a
 * std::optional. An empty optional indicates the end of the stream.
 *
 * The type of stream generated is the std;:optional::value_type of the UnaryFunc result, not the type of seed. 
*/
void example_finite_generator() {
    using namespace streamer;
    
    int counter = 0;
    std::vector<int> result = finite_generator([&counter]() { 
            ++counter; 
            if(counter < 5)
                return std::optional(counter * counter); 
            else
                return std::optional<int>();
        })
        | as_vector;


    std::vector<int> result2 = finite_generator(std::optional(5), [](auto x) {
            if(*x < 50)
                return std::optional(*x + *x); 
            else
                return std::optional<int>();
        })
        % as_vector();


    std::vector<int> expected = {1, 4, 9, 16};
    std::vector<int> expected2 = {5, 10, 20, 40, 80};

    assert(result == expected);
    assert(result2 == expected2);
}
