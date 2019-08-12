#ifndef STREAMER_BASE_FILTER_HPP
#define STREAMER_BASE_FILTER_HPP

#include "base.hpp"


namespace streamer {


class single_error : public streamer_error {
public:
    single_error(const std::string &what) : streamer_error(what) {}
    single_error(const char *what) : streamer_error(what) {}
};



namespace detail {


template<typename UnaryPred, typename T>
class filter_step : public step<T> {
public:
    filter_step(UnaryPred &&p, std::unique_ptr<step<T> > &&next) 
        : pred(std::move(p)), next_step(std::move(next)) {}

    std::optional<T> get() override {
        while(std::optional<T> value = next_step->get()) {
            if(pred(*value))
                return value;
        }
        return {};
    }
private:
    UnaryPred pred;
    std::unique_ptr<step<T> > next_step;
};



template<typename UnaryPred>
class first_if_t : public step_wrapper<first_if_t<UnaryPred> > {
public:
    first_if_t(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &) {
        return filter_step<UnaryPred, T>(std::move(pred), std::move(s)).get();
    }

private:
    UnaryPred pred;
};


class first_t : public detail::step_wrapper<first_t> {
public:
    constexpr first_t &operator()() noexcept { return *this; }

    template<typename UnaryPred>
    first_if_t<UnaryPred> operator()(UnaryPred pred) { return first_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &) {
        return s->get();
    }
};



template<typename UnaryPred>
class single_if_t : public step_wrapper<single_if_t<UnaryPred> > {
public:
    single_if_t(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use single(UnaryPred) on an unbounded stream");

        filter_step<UnaryPred, T> f(std::move(pred), std::move(s));
        std::optional<T> value = f.get();

        if(value && f.get())
            throw single_error("stream contains multiple values matching predicate");

        return value;
    }

private:
    UnaryPred pred;
};


class single_t : public detail::step_wrapper<single_t> {
public:
    constexpr single_t &operator()() noexcept { return *this; }

    template<typename UnaryPred>
    single_if_t<UnaryPred> operator()(UnaryPred pred) { return single_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use single on an unbounded stream");

        std::optional<T> value = s->get();
        if(value && s->get())
            throw single_error("stream contains more than a single value");

        return value;
    }
};



template<typename UnaryPred>
class last_if_t : public step_wrapper<last_if_t<UnaryPred> > {
public:
    last_if_t(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use last(UnaryPred) on an unbounded stream");

        std::optional<T> last;
        filter_step<UnaryPred, T> f(std::move(pred), std::move(s));

        while(std::optional<T> next = f.get()) {
            last = std::move(next);
        }
        return last;
    }

private:
    UnaryPred pred;
};


class last_t : public detail::step_wrapper<last_t> {
public:
    constexpr last_t &operator()() noexcept { return *this; }

    template<typename UnaryPred>
    last_if_t<UnaryPred> operator()(UnaryPred pred) { return last_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use last on an unbounded stream");

        std::optional<T> last;
        while(std::optional<T> next = s->get()) {
            last = std::move(next);
        }
        return last;
    }
};



} // namespace detail



template<typename UnaryPred>
class filter : public detail::step_wrapper<filter<UnaryPred> > {
public:
    filter(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &) {
        s.reset(new detail::filter_step<UnaryPred, T>(std::move(pred), std::move(s)));
        return std::move(st);
    }

private:
    UnaryPred pred;
};


template<typename UnaryPred>
auto exclude(UnaryPred p) {
    return filter([p](const auto &x) { return !p(x); });
}


static detail::first_t first;
static detail::last_t last;
static detail::single_t single;


namespace detail {
    inline void filter_unused_warnings() {
        first();
        last();
        single();
    }
} // namespace detail

} // namespace streamer

#endif
