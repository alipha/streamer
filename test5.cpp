#include <iostream>
#include <utility>
using namespace std;
 
class Foo {
public:
    Foo() {}
    Foo(const Foo &) { std::cout << "copy" << std::endl; }
    Foo(Foo &&) { std::cout << "move" << std::endl; }
};
 
template<typename T>
class Test {
public:
    Test(T x) : value(std::move(x)) {}
 
private:
    T value;
};
 
template<typename T>
Test<T> foo(T &&x) {
    return Test<T>(std::forward<T>(x));
}
 
int main() {
    Foo f;
    Test{f};
    foo(f);
    Test(std::move(f));
    foo(std::move(f));
    const Foo cf;
    Test{cf};
    foo(cf);
    Test{Foo()};
    foo(Foo());
    return 0;
}
