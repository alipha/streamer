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



template<typename Derived>
class opt_step_wrapper {
public:
    constexpr opt_step_wrapper() noexcept = default;

    constexpr Derived &get_derived() & noexcept { return static_cast<Derived&>(*this); }
    constexpr Derived &&get_derived() && noexcept { return static_cast<Derived&&>(*this); }

protected:
    opt_step_wrapper(const opt_step_wrapper<Derived> &) noexcept = default;
    opt_step_wrapper(opt_step_wrapper<Derived> &&) noexcept = default;
    opt_step_wrapper<Derived> &operator=(const opt_step_wrapper<Derived> &) noexcept = default;
    opt_step_wrapper<Derived> &operator=(opt_step_wrapper<Derived> &&) noexcept = default;
};



template<typename T>
struct or_default_value_t : opt_step_wrapper<or_default_value_t<T> > {
    or_default_value_t(T value) : val(std::move(value)) {}
    T val;
};


struct or_default_t : opt_step_wrapper<or_default_t> {
    or_default_t &operator()() { return *this; }

    template<typename T>
    or_default_value_t<T> operator()(T &&value) {
        return or_default_value_t<T>(std::forward<T>(value));
    }                                         
};



template<typename Func>
struct or_throw_func_t : opt_step_wrapper<or_throw_func_t<Func> > {                
    or_throw_func_t(Func f) : func(std::move(f)) {}   
    Func func;                                
};                                                       


struct or_throw_t : opt_step_wrapper<or_throw_t> {
    or_throw_t &operator()() { return *this; }

    template<typename Func>
    auto operator()(Func &&f) {
        return or_throw_func_t(func_or_except<Func>::get(f));
    }                                         
};

} // namespace detail



template<typename Func>
struct or_get : detail::opt_step_wrapper<or_get<Func> > {
    or_get(Func f) : func(std::move(f)) {}
    Func func;
};


static detail::or_default_t or_default;
static detail::or_throw_t or_throw;             


template<typename T, typename U>
T operator>>(std::optional<T> &&opt, detail::or_default_value_t<U> &&def) {
    return std::move(opt).value_or(std::move(def.val));
}


template<typename T>
T operator>>(std::optional<T> &&opt, const detail::or_default_t &) {
    if(opt)
        return *std::move(opt);
    else
        return T();
}


template<typename T, typename Func>
T operator>>(std::optional<T> &&opt, or_get<Func> &&getter) {
    return opt ? *opt : getter.func();
}


template<typename T, typename Func>
T operator>>(std::optional<T> &&opt, detail::or_throw_func_t<Func> &&thrower) {
    if(opt)
        return *std::move(opt);
    throw thrower.func();
}


template<typename T>
T operator>>(std::optional<T> &&opt, const detail::or_throw_t &) {
    return opt.value();
}



template<typename T, typename OptStep>
auto operator%(std::optional<T> &&opt, detail::opt_step_wrapper<OptStep> &step) {
    return std::move(opt) >> step.get_derived();
}

template<typename T, typename OptStep>
auto operator%(std::optional<T> &&opt, detail::opt_step_wrapper<OptStep> &&step) {
    return std::move(opt) >> std::move(step).get_derived();
}

template<typename T, typename OptStep>
auto operator|(std::optional<T> &&opt, detail::opt_step_wrapper<OptStep> &step) {
    return std::move(opt) >> step.get_derived();
}

template<typename T, typename OptStep>
auto operator|(std::optional<T> &&opt, detail::opt_step_wrapper<OptStep> &&step) {
    return std::move(opt) >> std::move(step).get_derived();
}



namespace detail {
    inline void optional_unused_warning() {
        or_default();
        or_throw();
    }
}

} // namespace streamer

#endif
