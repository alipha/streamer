#ifndef STREAMER_SET_HPP
#define STREAMER_SET_HPP

#include "base.hpp"
#include <functional>
#include <set>
#include <iterator>


namespace streamer {


class duplicate_set_key : public streamer_error {
public:
    duplicate_set_key(const std::string &what) : streamer_error(what) {}
    duplicate_set_key(const char *what) : streamer_error(what) {}
};


template<typename Comp>
class as_set_custom_t {
public:
    as_set_custom_t(Comp c, bool throw_on_dup) : comp(c), dup_throw(throw_on_dup) {}

    template<typename T>
    std::set<T, Comp> stream(streamer_t<T> &st, std::vector<T> &values) {
        if(!dup_throw)
            return std::set<T, Comp>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()), comp);

        std::set<T, Comp> out(comp);

        for(auto &value : values) {
            if(!out.insert(std::move(value))->second)
                throw duplicate_set_key("value already exists in set");
        }

        return out;
    }

private:
    Comp comp;
    bool dup_throw;
};


static class as_set_t {
public:
    as_set_t &operator()() { return *this; }

    template<typename Comp>
    as_set_custom_t<Comp> operator()(Comp comp, bool throw_on_dup = true) { 
        return as_set_custom_t<Comp>(comp, throw_on_dup); 
    }

    template<typename T>
    std::set<T> stream(streamer_t<T> &st, std::vector<T> &values) {
        return as_set_custom_t(std::less<T>(), false).stream(st, values);
    }
} as_set;


template<typename Comp>
class as_multiset_custom_t {
public:
    as_multiset_custom_t(Comp c) : comp(c) {}

    template<typename T>
    std::multiset<T, Comp> stream(streamer_t<T> &st, std::vector<T> &values) {
        return std::multiset<T, Comp>(
                std::make_move_iterator(values.begin()), 
                std::make_move_iterator(values.end()), 
                comp);
    }

private:
    Comp comp;
};


static class as_multiset_t {
public:
    as_multiset_t &operator()() { return *this; }

    template<typename Comp>
    as_multiset_custom_t<Comp> operator()(Comp comp) { 
        return as_multiset_custom_t<Comp>(comp); 
    }

    template<typename T>
    std::multiset<T> stream(streamer_t<T> &st, std::vector<T> &values) {
        return as_multiset_custom_t(std::less<T>()).stream(st, values);
    }
} as_multiset;


namespace detail {
    inline void set_unused_warnings() {
        as_set();
        as_multiset();
    }
}


} // namespace streamer

#endif