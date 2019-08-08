#ifndef STREAMER_REDUCE_HPP
#define STREAMER_REDUCE_HPP

#include "base.hpp"
#include <algorithm>
#include <optional>


template<typename BiFunc, typename T>
class fold_init_t : public detail::stream_manip<fold_init_t<BiFunc, T> > {
public:
    fold_init_t(BiFunc &&f, T &&init_value) : func(std::move(f)), init(std::move(init_value)) {}

    template<typename U>
    auto stream(streamer_t<U> &, std::vector<U> &values) {
        using R = typename detail::remove_ref_cv<decltype(func(std::move(init), std::move(values[0])))>::type;
        R result = std::move(init);

        for(auto &value : values)
            result = func(std::move(result), std::move(value));

        return result;
    }

private:
    BiFunc func;
    T init;
};


template<typename BiFunc>
class fold_t : public detail::stream_manip<fold_t<BiFunc> > {
public:
    fold_t(BiFunc &&f) : func(std::move(f)) {}

    template<typename U>
    auto stream(streamer_t<U> &, std::vector<U> &values) {
        using R = typename detail::remove_ref_cv<decltype(func(std::move(values[0]), std::move(values[0])))>::type;

        if(values.empty())
            return std::optional<R>();

        R result = std::move(values.front());

        for(auto &it = values.begin() + 1; it != values.end(); ++it)
            result = func(std::move(result), std::move(*it));

        return std::optional<R>(result);
    }

private:
    BiFunc func;
};


template<typename BiFunc>
fold_t<BiFunc> fold(BiFunc f) { return fold_t<BiFunc>(std::move(f)); }

template<typename BiFunc, typename T>
fold_init_t<BiFunc, T> fold(BiFunc f, T init_value) { 
    return fold_init_t<BiFunc, T>(std::move(f), std::move(init_value));
}



template<typename Comp>
class minimum_custom_t : public detail::stream_manip<minimum_custom_t<Comp> > {
public:
    minimum_custom_t(Comp &&p) : comp(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return fold([this](const auto &x, const auto &y) { return comp(y, x) ? y : x; });
    }

private:
    Comp comp;
};


static class minimum_t : public detail::stream_manip<minimum_t> {
public:
    minimum_t &operator()() { return *this; }

    template<typename Comp>
    minimum_custom_t<Comp> operator()(Comp comp) { return minimum_custom_t<Comp>(std::move(comp)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return minimum_custom_t(std::less<T>());
    }
} minimum;


template<typename Comp>
class maximum_custom_t : public detail::stream_manip<maximum_custom_t<Comp> > {
public:
    maximum_custom_t(Comp &&p) : comp(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return fold([this](const auto &x, const auto &y) { return comp(x, y) ? y : x; });
    }

private:
    Comp comp;
};


static class maximum_t : public detail::stream_manip<maximum_t> {
public:
    maximum_t &operator()() { return *this; }

    template<typename Comp>
    maximum_custom_t<Comp> operator()(Comp comp) { return maximum_custom_t<Comp>(std::move(comp)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return maximum_custom_t(std::less<T>());
    }
} maximum;


#endif
