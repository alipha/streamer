#ifndef STREAMER_BASE_HPP
#define STREAMER_BASE_HPP

#include "detail.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>


namespace streamer {

    
template<typename T>
struct identity {
    T operator()(T t) const { return t; }
};


class streamer_error : public std::runtime_error {
public:
    streamer_error(const std::string &what) : std::runtime_error(what) {}
    streamer_error(const char *what) : std::runtime_error(what) {}
};


class unbounded_stream : public streamer_error {
public:
    unbounded_stream(const std::string &what) : streamer_error(what) {}
    unbounded_stream(const char *what) : streamer_error(what) {}
};


template<typename T>
class streamer_t {
public:
    streamer_t(std::unique_ptr<detail::step<T> > &&h, bool unbound) noexcept : 
        head(std::move(h)),
        unbounded(unbound) {}
  
    template<typename Cont>
    streamer_t(Cont &cont) : streamer_t(std::begin(cont), std::end(cont)) {}

    template<typename Cont>
    streamer_t(Cont &&cont) : 
        head(new detail::cont_source<typename detail::remove_ref_cv<Cont>::type, T>(std::move(cont))),
        unbounded(false) {}

    template<typename It>
    streamer_t(It begin, It end) :
        head(new detail::it_source<typename detail::remove_ref_cv<It>::type, T>(std::move(begin), std::move(end))),
        unbounded(false) {}


    streamer_t(const streamer_t<T> &) = delete;
    streamer_t<T> &operator=(const streamer_t<T> &) = delete;

    streamer_t(streamer_t<T> &&) noexcept = default;
    streamer_t<T> &operator=(streamer_t<T> &&) noexcept = default;


    class iterator : public std::iterator<std::input_iterator_tag, T, std::size_t, T*, T&> {
    public:
        constexpr iterator() noexcept : input(nullptr), value() {}
        iterator(detail::step<T> *i) : input(i), value(std::move(input->get())) {}

        iterator &operator++() { 
            value = std::move(input->get());
            return *this;
        }

        iterator operator++(int) {
            iterator ret = *this;
            ++(*this);
            return ret;
        }

        constexpr bool operator==(const iterator &other) const noexcept { return !value == !other.value; }
        constexpr bool operator!=(const iterator &other) const noexcept { return !(*this == other); }
        constexpr T &&operator*() { return std::move(*value); }

    private:
        detail::step<T> *input;
        std::optional<T> value;
    };

    iterator begin() { return iterator(head.get()); }
    constexpr iterator end() noexcept { return iterator(); }

    bool unbound() const noexcept { return unbounded; }

private:
    template<typename Cont, typename Step>
    friend auto operator>>(Cont &&cont, detail::step_wrapper<Step> &step);

    template<typename Cont, typename Step>
    friend auto operator>>(Cont &&cont, detail::step_wrapper<Step> &&step);

    std::unique_ptr<detail::step<T> > head;
    bool unbounded;
};
  


template<typename It>
auto stream(It begin, It end) {
    return streamer_t<typename detail::remove_ref_cv<decltype(*begin)>::type>(begin, end);
}

template<typename T>
streamer_t<T> stream(const T *p, std::size_t n) {
    return streamer_t<T>(p, p + n);
}

template<typename T, std::size_t N>
streamer_t<T> stream(const T (&a)[N]) {
    return streamer_t<T>(a, a + N);
}

template<typename Cont>
auto stream(Cont &cont) {
    return stream(std::begin(cont), std::end(cont));
}

template<typename Cont>
auto stream(Cont &&cont) {
    using T = typename detail::remove_ref_cv<decltype(*std::begin(cont))>::type; 
    return streamer_t<T>(std::move(cont));
}

template<typename T>
streamer_t<T> &&stream(streamer_t<T> &&st) noexcept {
    return std::move(st);
}


template<typename It>
auto stream_move(It begin, It end) {
    return stream(std::make_move_iterator(begin), std::make_move_iterator(end));
}

template<typename T>
streamer_t<T> stream_move(T *p, std::size_t n) {
    return stream_move(p, p + n);
}

template<typename T, std::size_t N>
streamer_t<T> stream_move(T (&a)[N]) {
    return stream_move(a, a + N);
}

template<typename Cont>
auto stream_move(Cont &cont) {
    return stream_move(std::begin(cont), std::end(cont));
}



template<typename Cont, typename Step>
auto operator>>(Cont &&cont, detail::step_wrapper<Step> &step) {
    auto st = stream(std::forward<Cont>(cont));
    return step.get_derived().stream(st, st.head, st.unbounded);
}

template<typename Cont, typename Step>
auto operator>>(Cont &&cont, detail::step_wrapper<Step> &&step) {
    return std::forward<Cont>(cont) >> step;
}



template<typename Cont, typename Step>
auto operator|(Cont &&cont, detail::step_wrapper<Step> &step) {
    return std::forward<Cont>(cont) >> step;
}

template<typename Cont, typename Step>
auto operator|(Cont &&cont, detail::step_wrapper<Step> &&step) {
    return std::forward<Cont>(cont) >> std::move(step);
}



template<typename Cont, typename Step>
auto operator%(Cont &&cont, detail::step_wrapper<Step> &step) {
    return std::forward<Cont>(cont) >> step;
}

template<typename Cont, typename Step>
auto operator%(Cont &&cont, detail::step_wrapper<Step> &&step) {
    return std::forward<Cont>(cont) >> std::move(step);
}



} // namespace streamer


#endif
