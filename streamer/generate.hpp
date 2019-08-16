#ifndef STREAMER_GENERATE_HPP
#define STREAMER_GENERATE_HPP

#include "base.hpp"


namespace streamer {


namespace detail {


template<typename GenFunc, typename T>
class gen_source : public step<T> {
public:
    gen_source(GenFunc &&f) : func(std::move(f)) {}

    std::optional<T> get() override {
        return {func()};
    }
private:
    GenFunc func;
};


template<typename UnaryFunc, typename T>
class func_source : public step<T> {
public:
    func_source(T &&seed, UnaryFunc &&f) : value(std::move(seed)), func(std::move(f)) {}

    std::optional<T> get() override {
        std::optional<T> ret = {value};
        value = func(std::move(value));
        return ret;
    }
private:
    UnaryFunc func;
    T value;
};


template<typename GenFunc, typename T>
class bound_gen_source : public step<T> {
public:
    bound_gen_source(GenFunc &&f) : func(std::move(f)) {}

    std::optional<T> get() override {
        return func();
    }
private:
    GenFunc func;
};


template<typename UnaryFunc, typename T>
class bound_func_source : public step<T> {
public:
    bound_func_source(std::optional<T> &&seed, UnaryFunc &&f) : value(std::move(seed)), func(std::move(f)) {}

    std::optional<T> get() override {
        if(!value)
            return {};
        std::optional<T> ret = value;
        value = func(std::move(value));
        return ret;
    }
private:
    UnaryFunc func;
    std::optional<T> value;
};



} // namespace detail



template<typename GenFunc>
auto stream_generate(GenFunc f) {
    using T = typename detail:remove_ref_cv<decltype(f())>::type;
    return streamer_t<T>(std::make_unique<detail::gen_source<GenFunc, T> >(std::move(f)), true);
}


template<typename T, typename UnaryFunc>
auto stream_generate(T seed, UnaryFunc f) {
    using U = typename detail:remove_ref_cv<decltype(f(std::move(seed)))>::type;
    return streamer_t<U>(std::make_unique<detail::func_source<UnaryFunc, T> >(std::move(seed), std::move(f)), true);
}


template<typename T>
auto stream_range(T begin, T end) {
    return stream_generate(std::optional<T>(std::move(begin)), [end](auto x) { return std::less(*x, 
        if(*x < end)
            return x;
        else
    });
}


template<typename T>
auto stream_range(T begin, T end, T step) {
}


template<typename T, typename UnaryFunc>
auto stream_range(T begin, T end, UnaryFunc step) {
}


} // namespace streamer

#endif
