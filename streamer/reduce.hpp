#ifndef STREAMER_REDUCE_HPP
#define STREAMER_REDUCE_HPP

#include "base.hpp"


namespace streamer {


namespace detail {


template<typename BiFunc, typename T>
class fold_init_t : public step_wrapper<fold_init_t<BiFunc, T> > {
public:
    fold_init_t(BiFunc &&f, T &&init_value) : func(std::move(f)), init(std::move(init_value)) {}

    template<typename U>
    auto stream(streamer_t<U> &, std::unique_ptr<step<U> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use fold(BiFunc, T) on an unbounded stream");

        using R = typename remove_ref_cv<decltype(func(*s->get(), *s->get()))>::type;

        R result = std::move(init);

        while(std::optional<U> value = s->get())
            result = func(std::move(result), *std::move(value));

        return result;
    }

private:
    BiFunc func;
    T init;
};


template<typename BiFunc>
class fold_t : public step_wrapper<fold<BiFunc> > {
public:
    fold_t(BiFunc &&f) : func(std::move(f)) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use fold(BiFunc) on an unbounded stream");

        using R = typename remove_ref_cv<decltype(func(*s->get(), *s->get()))>::type;

        std::optional<T> value = s->get();
        if(!value)
            return std::optional<R>();

        R result = *std::move(value);

        while((value = s->get()))
            result = func(std::move(result), *std::move(value));

        return std::optional<R>(std::move(result));
    }

private:
    BiFunc func;
};



template<typename Comp>
class minimum_custom_t : public step_wrapper<minimum_custom_t<Comp> > {
public:
    minimum_custom_t(Comp &&p) : comp(std::move(p)) {}

    template<typename T>
    auto stream(streamer_t<T> st&, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use minimum(Comp) on an unbounded stream");

        return fold([this](const auto &x, const auto &y) { return comp(y, x) ? y : x; })
            .stream(st, s, unbounded);
    }

private:
    Comp comp;
};


class minimum_t : public step_wrapper<minimum_t> {
public:
    minimum_t &operator()() { return *this; }

    template<typename Comp>
    auto operator()(Comp comp) {
        return minimum_custom_t(member_comparer(std::move(comp)));
    }

    template<typename KeyFunc, typename Comp>
    auto operator()(KeyFunc k, Comp comp) {
        return minimum_custom_t(member_comparer_custom(std::move(k), std::move(comp)));
    }

    template<typename T>
    std::optional<T> stream(streamer_t<T> st&, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use minimum on an unbounded stream");

        return minimum_custom_t<std::less<T> >(std::less<T>()).stream(st, s, unbounded);
    }
};


} // namespace detail


template<typename BiFunc, typename T>
auto fold(BiFunc func, T init) {
    return detail::fold_init_t(std::move(func), std::move(init));
}

template<typename BiFunc>
auto fold(Bifunc func) {
    return detail::fold_t<BiFunc>(std::move(func));
}


static detail::minimum_t minimum;


namespace detail {
    inline void reduce_unused_warnings() {
        minimum();
    }
} // namespace detail


} // namespace streamer

#endif
