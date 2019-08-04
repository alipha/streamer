#ifndef STREAMER_DETAIL_HPP
#define STREAMER_DETAIL_HPP

#include <functional>
#include <type_traits>


namespace streamer {
namespace detail {


template<typename T>
struct remove_ref_cv {
    typedef typename std::remove_reference<typename std::remove_cv<T>::type>::type type;
};



template<typename UnaryFunc>
auto member_mapper(UnaryFunc &&f) { return f; }

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
auto member_comparer(U T::*p, Comp &&comp) {
    return [p, comp](const T &left, const T &right) { return comp(left.*p, right.*p); };
}

template<typename T, typename U, typename Comp>
auto member_comparer(U (T::*p)() const, Comp &&comp) {
    return [p, comp](const T &left, const T &right) { return comp((left.*p)(), (right.*p)()); };
}

template<typename T, typename U, typename Comp>
auto member_comparer(U (T::*p)() const noexcept, Comp &&comp) {
    return [p, comp](const T &left, const T &right) { return comp((left.*p)(), (right.*p)()); };
}

  
} // namespace detail
} // namespace streamer

#endif
