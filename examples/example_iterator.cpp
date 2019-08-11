#include <array>
#include <cassert>
#include <vector>

/*
 * streamer_t<T>::begin and streamer_t<T>::end return input iterators representing the 
 * start of the stream and that the stream has no more elements, respectively.
 *
 * Iterating over a stream is destructive and iterating over the stream removes
 * elements from the stream.
 * 
*/
void example_iterator() {
    using namespace streamer;

    std::array<int, 5> input = {56, 3, 23, 100, 42};
    
    streamer_t<int> st = stream(input);
    streamer_t<int> st2 = stream(input);

    std::vector<int> out, out2, out3;


    for(streamer_t<int>::iterator it = st.begin(); it != st.end(); it++) {
        out.push_back(*it);
    }

    for(auto it = std::begin(st2); it != std::end(st2); ++it) {
        out2.push_back(*it);
    }

    for(auto value : input | mapping([](auto x) { return x * 10; })) {
        out3.push_back(value);
    }

    std::vector<int> expected = {56, 3, 23, 100, 42};
    auto expected2 = expected;
    std::vector<int> expected3 = {560, 30, 230, 1000, 420};

    assert(out == expected);
    assert(out2 == expected2);
    assert(out3 == expected3);
}
