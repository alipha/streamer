#ifndef STREAMER_BASE_HPP
#define STREAMER_BASE_HPP

#include "detail.hpp"
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


namespace streamer {


template<typename T>
class streamer_t;


namespace detail {

template<typename Cont>
auto range(Cont &&c);

template<typename U>
auto range(streamer_t<U> &&s);

template<typename U>
auto range(streamer_t<U> &s);

} // namespace detail


    
template<typename T>
struct identity {
    T operator()(T t) const { return t; }
};


class streamer_error : public std::runtime_error {
public:
    streamer_error(const std::string &what) : std::runtime_error(what) {}
    streamer_error(const char *what) : std::runtime_error(what) {}
};



template<typename T>
class streamer_t {
public:
    typedef typename std::vector<T>::value_type value_type;


    streamer_t() noexcept : values() {}

    explicit streamer_t(std::vector<T> &&v) noexcept : values(std::move(v)) {}

    template<typename It>
    streamer_t(It begin, It end) : values(begin, end) {}


    streamer_t(streamer_t &&) = default;
    streamer_t &operator=(streamer_t &&) = default;
    
    streamer_t(const streamer_t &) = delete;
    streamer_t &operator=(const streamer_t &) = delete;

private:
    template<typename Cont, typename Manip>
    friend auto operator>>(Cont &&c, detail::stream_manip<Manip> &manip);

    template<typename Cont, typename Manip>
    friend auto operator>>(Cont &&c, detail::stream_manip<Manip> &&manip);

    friend auto detail::range<>(streamer_t<T> &&s);
    friend auto detail::range<>(streamer_t<T> &s);

    std::vector<T> values;
};


namespace detail {

template<typename Cont>
auto range(Cont &&c) {
    return std::make_pair(std::begin(c), std::end(c));
}

template<typename U>
auto range(streamer_t<U> &&s) {
    return std::make_pair(s.values.begin(), s.values.end());
}

template<typename U>
auto range(streamer_t<U> &s) {
    return std::make_pair(s.values.begin(), s.values.end());
}

} // namespace detail


template<typename T>
streamer_t<T> &&stream(streamer_t<T> &&st) noexcept {
    return std::move(st);
}

template<typename T>
streamer_t<T> stream(std::vector<T> &&v) noexcept {
    return streamer_t<T>(std::move(v));
}

template<typename T>
streamer_t<T> stream(const T *p, std::size_t n) {
    return streamer_t<T>(p, p + n);
}

template<typename T, std::size_t N>
streamer_t<T> stream(const T (&a)[N]) {
    return streamer_t<T>(a, a + N);
}

/* TODO: add Cont && and Cont & */
template<typename Cont>
auto stream(const Cont &c) {
    return streamer_t<typename detail::remove_ref_cv<decltype(*std::begin(c))>::type>(std::begin(c), std::end(c));
}

template<typename It>
auto stream(It begin, It end) {
    return streamer_t<typename detail::remove_ref_cv<decltype(*begin)>::type>(begin, end);
}

template<typename T>
streamer_t<T> stream_of(std::initializer_list<T> init) noexcept {
    return streamer_t<T>(std::vector<T>(init));
}



template<typename Cont, typename Manip>
auto operator>>(Cont &&c, detail::stream_manip<Manip> &manip) {
    auto st = stream(std::forward<Cont>(c));
    return manip.get_derived().stream(st, st.values);
}

template<typename Cont, typename Manip>
auto operator%(Cont &&c, detail::stream_manip<Manip> &manip) {
    return std::forward<Cont>(c) >> manip;
}

template<typename Cont, typename Manip>
auto operator>>(Cont &&c, detail::stream_manip<Manip> &&manip) {
    auto st = stream(std::forward<Cont>(c));
    return manip.get_derived().stream(st, st.values);
}

template<typename Cont, typename Manip>
auto operator|(Cont &&c, detail::stream_manip<Manip> &&manip) {
    return std::forward<Cont>(c) >> std::move(manip);
}

/*
template<typename U, typename Operation>
auto operator>>(streamer_t<U> &&st, Operation &&op) {
    return op.stream(st, st.values);
}

template<typename T, typename Operation>
auto operator%(streamer_t<T> &&st, Operation &&op) { return std::move(st) >> op; }
*/

}  // namespace streamer


#endif
