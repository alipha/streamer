#include <iostream>
#include <vector>
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


int main() {
    std::vector<int> v;
    v >> except([](auto x) { return x; });
    return 0;
}

