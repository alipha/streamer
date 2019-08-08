#include "streamer/list.hpp"
#include <iostream>


using namespace streamer;


int main () {
    int test[] = {5, 25, 21, 48};
    std::list<int> out = test >> as_list;
    std::list<int> out2 = test >> as_list();

    for(auto &x : out)
        std::cout << x << std::endl;

    for(auto &x : out2)
        std::cout << x << std::endl;

    return 0;
}
