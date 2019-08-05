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
auto range(const Cont &c);

template<typename U>
auto range(const streamer_t<U> &s);

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
    template<typename U, typename Operation>
    friend auto operator>>(streamer_t<U> &&st, Operation &&op);

    template<typename Cont>
    friend auto detail::range(const Cont &c);

    template<typename U>
    friend auto detail::range(const streamer_t<U> &s);

    std::vector<T> values;
};


namespace detail {

template<typename Cont>
auto range(const Cont &c) {
    return std::make_pair(std::begin(c), std::end(c));
}

template<typename U>
auto range(const streamer_t<U> &s) {
    return std::make_pair(s.values.begin(), s.values.end());
}

} // namespace detail


template<typename T>
streamer_t<T> streamer(std::vector<T> &&v) noexcept {
    return streamer_t<T>(std::move(v));
}

template<typename T>
streamer_t<T> streamer(const T *p, std::size_t n) {
    return streamer_t<T>(p, p + n);
}

template<typename T, std::size_t N>
streamer_t<T> streamer(const T (&a)[N]) {
    return streamer_t<T>(a, a + N);
}

template<typename Cont>
auto streamer(Cont &&c) {
    return streamer_t<typename detail::remove_ref_cv<decltype(*std::begin(c))>::type>(std::begin(c), std::end(c));
}

template<typename It>
auto streamer(It begin, It end) {
    return streamer_t<typename detail::remove_ref_cv<decltype(*begin)>::type>(begin, end);
}

template<typename T>
streamer_t<T> streamer_of(std::initializer_list<T> init) noexcept {
    return streamer_t<T>(std::vector<T>(init));
}



template<typename U, typename Operation>
auto operator>>(streamer_t<U> &&st, Operation &&op) {
    return op.stream(st, st.values);
}

template<typename T, typename Operation>
auto operator%(streamer_t<T> &&st, Operation &&op) { return std::move(st) >> op; }


}  // namespace streamer


#endif
