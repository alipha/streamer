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
    func_source(T &&seed, UnaryFunc &&f) : next(std::move(seed)), func(std::move(f)) {}

    std::optional<T> get() override {
        std::optional<T> value = {next};
        next = func(std::move(next));
        return value;
    }
private:
    T next;
    UnaryFunc func;
};


template<typename T, typename UnaryFunc, typename Comp>
class range_source : public step<T> {
public:
    range_source(T &&b, T &&e, UnaryFunc &&f, Comp &&c) 
        : begin(b), next(std::move(b)), end(std::move(e)), func(std::move(f)), comp(std::move(c)) {}

    std::optional<T> get() override {
        if(!comp(next, end) || comp(next, begin))
            return {};
        T value = next;
        next = func(std::move(next));
        return {value};
    }
private:
    T begin;
    T next;
    T end;
    UnaryFunc func;
    Comp comp;
};


} // namespace detail



template<typename GenFunc>
auto generator(GenFunc f) {
    using T = typename detail::remove_ref_cv<decltype(f())>::type;
    return streamer_t<T>(std::make_unique<detail::gen_source<GenFunc, T> >(std::move(f)), true);
}


template<typename T, typename UnaryFunc>
auto generator(T seed, UnaryFunc f) {
    using U = typename detail::remove_ref_cv<decltype(f(std::move(seed)))>::type;
    return streamer_t<U>(std::make_unique<detail::func_source<UnaryFunc, U> >(std::move(seed), std::move(f)), true);
}



template<typename T, typename UnaryFunc, typename KeyFunc, typename Comp>
streamer_t<T> range(T begin, T end, UnaryFunc step, KeyFunc keyFunc, Comp comp) {
    auto c = detail::member_comparer(std::move(keyFunc), std::move(comp));
    auto src = new detail::range_source<T, UnaryFunc, decltype(c)>(std::move(begin), std::move(end), std::move(step), std::move(c));
    using SrcType = typename std::pointer_traits<decltype(src)>::element_type;
    return streamer_t<T>(std::unique_ptr<SrcType>(src), false);
}


template<typename T, typename UnaryFunc, typename Comp>
streamer_t<T> range(T begin, T end, UnaryFunc step, Comp comp) {
    auto c = detail::member_comparer(std::move(comp));
    auto src = new detail::range_source<T, UnaryFunc, decltype(c)>(std::move(begin), std::move(end), std::move(step), std::move(c));
    using SrcType = typename std::pointer_traits<decltype(src)>::element_type;
    return streamer_t<T>(std::unique_ptr<SrcType>(src), false);
}


template<typename T, typename KeyFunc, typename Comp>
streamer_t<T> range(T begin, T end, T step, KeyFunc keyFunc, Comp comp) {
    auto c = detail::member_comparer(std::move(keyFunc), std::move(comp));
    return range(std::move(begin), std::move(end), [step](auto x) { return x += step; }, std::move(c));
}

template<typename T, typename Comp>
streamer_t<T> range(T begin, T end, T step, Comp comp) {
    auto c = detail::member_comparer(std::move(comp));
    return range(std::move(begin), std::move(end), [step](auto x) { return x += step; }, std::move(c));
}

template<typename T, typename UnaryFunc>
auto range(T begin, T end, UnaryFunc step) {
    return range(std::move(begin), std::move(end), std::move(step), std::less<T>());
}

template<typename T>
auto range(T begin, T end, T step) {
    return range(std::move(begin), std::move(end), std::move(step), std::less<T>());
}

template<typename T>
auto range(T begin, T end) {
    return range(std::move(begin), std::move(end), [](auto x) { return ++x; }, std::less<T>());
}


template<typename T, typename UnaryFunc>
auto range_desc(T begin, T end, UnaryFunc step) {
    return range(std::move(begin), std::move(end), std::move(step), std::greater<T>());
}

template<typename T>
auto range_desc(T begin, T end, T step) {
    return range(std::move(begin), std::move(end), std::move(step), std::greater<T>());
}

template<typename T>
auto range_desc(T begin, T end) {
    return range(std::move(begin), std::move(end), [](auto x) { return --x; }, std::greater<T>());
}


} // namespace streamer

#endif
