#ifndef STREAMER_OPTIONAL_HPP
#define STREAMER_OPTIONAL_HPP

#include "base.hpp"
#include <optional>


namespace streamer {


namespace detail {

template<typename Func, bool IsEx = std::is_base_of<std::exception, Func>::value>
struct func_or_except {
    static auto get(Func f) { return f; }
};

template<typename Func>
struct func_or_except<Func, true> {
    static auto get(const Func &ex) { return [&ex]()->auto& { return ex; }; }
};

} // namespace detail


template<typename T>
struct or_default {
    or_default(T value) : val(std::move(value)) {}
    T val;
};


template<typename T, typename U>
T operator>>(std::optional<T> &&opt, or_default<U> &&def) {
    return std::move(opt).value_or(std::move(def.val));
}


template<typename Func>
struct or_get {
    or_get(Func f) : func(std::move(f)) {}
    Func func;
};


template<typename T, typename Func>
T operator>>(std::optional<T> &&opt, or_get<Func> &&def) {
    return opt ? *opt : def.func();
}


template<typename Func>
struct or_throw_func_t {                
    or_throw_func_t(Func f) : func(std::move(f)) {}   
    Func func;                                
};                                                       
                                                          
static struct or_throw_t {  
    template<typename Func>
    auto operator()(Func &&f) {
        return or_throw_func_t(detail::func_or_except<Func>::get(f));
    }                                         
} or_throw;             


template<typename T, typename Func>
T operator>>(std::optional<T> &&opt, or_throw_func_t<Func> &&def) {
    if(opt)
        return *opt;
    throw def.func();
}


namespace detail {
    inline void optional_unused_warning() {
        (void)or_throw;
    }
}

} // namespace streamer

#endif
