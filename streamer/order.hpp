#ifndef STREAMER_ORDER_HPP
#define STREAMER_ORDER_HPP

#include "base.hpp"
#include <algorithm>
#include <cstddef>
#include <string>


namespace streamer {


class not_enough_elements : public streamer_error {
public:
    not_enough_elements(const std::string &what) : streamer_error(what) {}
    not_enough_elements(const char *what) : streamer_error(what) {}
};


static class backwards_t : public detail::stream_manip<backwards_t> {
public:
    backwards_t &operator()() { return *this; }

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        std::reverse(values.begin(), values.end());
        return std::move(st); 
    }
} backwards;


class take : public detail::stream_manip<take> {
public:
    take(std::size_t amount, bool require_at_least = false) : n(amount), require_n(require_at_least) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        if(n < values.size())
            values.resize(n);
        else if(n > values.size() && require_n)
            throw not_enough_elements("streamer had only " + std::to_string(values.size()) 
                    + " elements when take requested " + std::to_string(n));
        return std::move(st);
    }

private:
    std::size_t n;
    bool require_n;
};


class skip : public detail::stream_manip<skip> {
public:
    skip(std::size_t amount, bool require_at_least = false) : n(amount), require_n(require_at_least) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        if(n <= values.size())
            values.erase(values.begin(), values.begin() + n);
        else if(require_n)
            throw not_enough_elements("streamer had only " + std::to_string(values.size()) 
                    + " elements when skip requested " + std::to_string(n));

        return std::move(st);
    }

private:
    std::size_t n;
    bool require_n;
};


template<typename Comp>
class sorted_custom_t : public detail::stream_manip<sorted_custom_t<Comp> > {
public:
    sorted_custom_t(Comp &&c) : comp(std::move(c)) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        std::sort(values.begin(), values.end(), std::move(comp));
        return std::move(st);
    }

private:
    Comp comp;
};


static class sorted_t : public detail::stream_manip<sorted_t> {
public:
    sorted_t &operator()() { return *this; }

    template<typename Comp>
    auto operator()(Comp comp) { 
        return sorted_custom_t(detail::member_comparer(std::move(comp))); 
    }

    template<typename KeyFunc, typename Comp>
    auto operator()(KeyFunc k, Comp comp) { 
        return sorted_custom_t(detail::member_comparer_custom(std::move(k), std::move(comp))); 
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
