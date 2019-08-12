#ifndef STREAMER_BASE_ORDER_HPP
#define STREAMER_BASE_ORDER_HPP

#include "base.hpp"


namespace streamer {


namespace detail {


template<typename T>
class take_step : public step<T> {
public:
    take_step(std::size_t amount, std::unique_ptr<step<T> > &&next) 
        : n(amount), next_step(std::move(next)) {}

    std::optional<T> get() override {
        if(n == 0)
            return {};

        n--;
        return next_step->get();
    }

private:
    std::size_t n;
    std::unique_ptr<step<T> > next_step;
};


template<typename T>
class skip_step : public step<T> {
public:
    skip_step(std::size_t amount, std::unique_ptr<step<T> > &&next) 
        : n(amount), next_step(std::move(next)) {}

    std::optional<T> get() override {
        while(value = next_step->get() && n > 0) {
            n--;
        }

        return value;
    }
private:
    std::size_t n;
    std::unique_ptr<step<T> > next_step;
};


} // namespace detail


class take : public detail::step_wrapper<take> {
public:
    take(std::size_t amount) : n(amount) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &) {
        s.reset(new detail::take_step<T>(n, std::move(s)));
        return std::move(st);
    }

private:
    std::size_t n;
};


class skip : public detail::step_wrapper<skip> {
public:
    skip(std::size_t amount) : n(amount) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &) {
        s.reset(new detail::skip_step<T>(n, std::move(s)));
        return std::move(st);
    }

private:
    std::size_t n;
};


} // namespace streamer

#endif
