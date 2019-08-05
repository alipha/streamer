#ifndef STREAMER_ORDER_HPP
#define STREAMER_ORDER_HPP

#include "base.hpp"
#include <algorithm>
#include <cstddef>


namespace streamer {


static class backwards_t {
public:
    backwards_t &operator()() { return *this; }

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        std::reverse(values.begin(), values.end());
        return std::move(st); 
    }
} backwards;


class take {
public:
    take(std::size_t amount) : n(amount) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        if(n > values.size())
            values.resize(n);
        return std::move(st);
    }

private:
    std::size_t n;
};


class skip {
public:
    skip(std::size_t amount) : n(amount) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        if(n < values.size())
            values.erase(values.begin(), values.begin() + n);
        else
            values.clear();
        return std::move(st);
    }

private:
    std::size_t n;
};


template<typename Comp>
class sorted_custom_t {
public:
    sorted_custom_t(Comp c) : comp(c) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        std::sort(values.begin(), values.end(), comp);
        return std::move(st);
    }

private:
    Comp comp;
};


static class sorted_t {
public:
    sorted_t &operator()() { return *this; }

    template<typename Comp>
    sorted_custom_t<Comp> operator()(Comp comp) { return sorted_custom_t<Comp>(comp); }

    template<typename KeyFunc, typename Comp>
    auto operator()(KeyFunc k, Comp comp) { 
        return sorted_custom_t(detail::member_comparer(k, comp)); 
    }

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        std::sort(values.begin(), values.end());
        return std::move(st); 
    }
} sorted;


namespace detail {
    inline void sorted_unused_warnings() {
        backwards();
        sorted();
    }
}


}  // namespace streamer

#endif
