#include <iostream>
#include <vector>
#include <utility>
//using namespace std;


template<typename Derived>
class stream_manip {
public:
    Derived &get_derived() { return static_cast<Derived&>(*this); }
};

template<typename Func>
class except : public stream_manip<except<Func>> {
public:
    except(Func f) : func(f) {}
    
    template<typename T>
    void perform(std::vector<T> &v) {}
    
private:
    Func func;
};

template<typename Cont, typename Manip>
auto operator>>(Cont &&c, stream_manip<Manip> &&manip) {
    Manip &derived = manip.get_derived();
    derived.perform(c);
}


class Foo {
public:
    Foo(int v) : value(v) { std::cout << "foo default" << std::endl; }
    Foo(const Foo &f) : value(f.value) { std::cout << "foo copy" << std::endl; }
    Foo(Foo &&f) : value(f.value) { std::cout << "foo move" << std::endl; }
    int value;
};

class Bar {
public:
    Bar(int v) : value(v) { std::cout << "bar default" << std::endl; }
    Bar(const Bar &b) : value(b.value) { std::cout << "bar copy" << std::endl; }
    int value;
};


template<typename T>
class Test {
public:
    Test(T t) : value(t) {}
    //Test(const T &t) : value(t) {}
    //Test(T &t) : value(t) {}
    //Test(T &&t) : value(std::move(t)) {}

private:
    T value;
};


Foo getFoo() { return Foo(5); }
Bar getBar() { return Bar(6); }

template<typename T>
void func(T &&t) { T loc(std::forward<T>(t)); std::cout << "func: " << loc.value << std::endl; }


int main() {
    Foo f(1);
    Bar b(2);
    std::cout << "1" << std::endl;
    func(f);
    std::cout << "2" << std::endl;
    func(Foo(3));
    std::cout << "3" << std::endl;
    func(b);
    std::cout << "4" << std::endl;
    func(Bar(4));
    std::cout << "5" << std::endl;
    func(getFoo());
    std::cout << "6" << std::endl;
    func(getBar());
    std::cout << "7" << std::endl;
    const Foo f2(7);
    func(f2);
    std::cout << "8" << std::endl;
    const Bar b2(8);
    func(b2);

    std::vector<int> v;
    v >> except([](auto x) { return x; });
    return 0;
}

