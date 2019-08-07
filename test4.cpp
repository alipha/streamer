#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
using namespace std;


template<typename Func, bool IsEx = std::is_base_of<std::exception, Func>::value>
struct func_or_except {
    static auto get(Func f) { return f; }
};

template<typename Func>
struct func_or_except<Func, true> {
    static auto get(const Func &ex) { return [&ex]()->auto& { return ex; }; }
};


template<typename Func>
struct or_throw_func_t {                
    or_throw_func_t(Func f) : func(std::move(f)) {}   
    Func func;                                
};                                                       
                                                          
static struct or_throw_t {  
    template<typename Func>
    auto operator()(Func &&f) {
        return or_throw_func_t(func_or_except<Func>::get(f));
    }                                         
    /*auto operator()(const std::exception &ex) {                     
        return or_throw_func_t([&ex]()->auto& { return ex; });  
    }                                                              
                                                                   
    template<typename Func>                                         
    or_throw_func_t<Func> operator()(Func f) {    
        return or_throw_func_t<Func>(std::move(f));    
    } */                                        
} or_throw;             


int main() {
    //auto t = or_throw(std::runtime_error("runtime"));
    //auto t2 = or_throw(std::logic_error("logic"));
    //auto t3 = or_throw([]() { return std::runtime_error("func runtime"); });
    
    try {
        throw or_throw(std::runtime_error("runtime")).func();
    } catch(const std::runtime_error &e) {
        cout << e.what() << endl;
    }

    /*
    try {
        throw t2.func();
    } catch(const std::runtime_error &e) {
        cout << e.what() << endl;
    }
    */
    try {
        throw or_throw([]() { return std::runtime_error("func runtime"); }).func();
    } catch(const std::runtime_error &e) {
        cout << e.what() << endl;
    }
    return 0;
}

