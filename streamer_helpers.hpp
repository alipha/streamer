#ifndef STREAMER_HELPERS_HPP
#define STREAMER_HELPERS_HPP

#include <functional>
#include <string>
#include <stdexcept>


class streamer_error : public std::runtime_error {
public:
    streamer_error(const std::string &what) : std::runtime_error(what) {}
    streamer_error(const char *what) : std::runtime_error(what) {}
};

class single_error : public streamer_error {
public:
    single_error(const std::string &what) : streamer_error(what) {}
    single_error(const char *what) : streamer_error(what) {}
};

class duplicate_set_key : public streamer_error {
public:
    duplicate_set_key(const std::string &what) : streamer_error(what) {}
    duplicate_set_key(const char *what) : streamer_error(what) {}
};

class duplicate_map_key : public streamer_error {
public:
    duplicate_map_key(const std::string &what) : streamer_error(what) {}
    duplicate_map_key(const char *what) : streamer_error(what) {}
};



template<typename Func>
auto member_mapper(const Func &f) { return f; }

template<typename T, typename U>
auto member_mapper(U T::*p) {
    return [p](const T &obj) { return obj.*p; };
}

template<typename T, typename U>
auto member_mapper(U (T::*p)() const) {
    return [p](const T &obj) { return (obj.*p)(); };
}

template<typename T, typename U>
auto member_mapper(U (T::*p)() const noexcept) {
    return [p](const T &obj) { return (obj.*p)(); };
}



template<typename T, typename U>
auto member_comparer(U T::*p) { return member_comparer(p, std::less<U>()); }

template<typename T, typename U>
auto member_comparer(U (T::*p)() const) { return member_comparer(p, std::less<U>()); }

template<typename T, typename U, typename Comp>
auto member_comparer(U T::*p, const Comp &comp) {
    return [p, comp](const T &left, const T &right) { return comp(left.*p, right.*p); };
}

template<typename T, typename U, typename Comp>
auto member_comparer(U (T::*p)() const, Comp comp) {
    return [p, comp](const T &left, const T &right) { return comp((left.*p)(), (right.*p)()); };
}

template<typename T, typename U, typename Comp>
auto member_comparer(U (T::*p)() const noexcept, Comp comp) {
    return [p, comp](const T &left, const T &right) { return comp((left.*p)(), (right.*p)()); };
}



template<typename T>
struct identity {
    T operator()(T t) const { return t; }
};


#endif
