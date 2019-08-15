#ifndef STREAMER_BASE_ORDER_HPP
#define STREAMER_BASE_ORDER_HPP

#include "base.hpp"
#include <deque>


namespace streamer {


namespace detail {


class reversed_t : public detail::step_wrapper<reversed_t> {
public:
    constexpr reversed_t &operator()() noexcept { return *this; }

    template<typename T>
    streamer_t<T> stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use reversed on an unbounded stream");

        std::deque<T> out;
        while(std::optional<T> value = s->get())
            out.push_front(*std::move(value));

        s.reset();
        return streamer_t<T>(std::move(out));
    }
};


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
        std::optional<T> value;
        while((value = next_step->get()) && n > 0) {
            n--;
        }
        return value;
    }
private:
    std::size_t n;
    std::unique_ptr<step<T> > next_step;
};


template<typename UnaryPred, typename T>
class take_while_step : public step<T> {
public:
    take_while_step(UnaryPred &&p, std::unique_ptr<step<T> > &&next) 
        : pred(std::move(p)), next_step(std::move(next)) {}

    std::optional<T> get() override {
        if(std::optional<T> value = next_step->get() && pred(*value))
            return value;
        else
            return {};
    }

private:
    UnaryPred pred;
    std::unique_ptr<step<T> > next_step;
};


template<typename UnaryPred, typename T>
class skip_while_step : public step<T> {
public:
    skip_while_step(UnaryPred &&p, std::unique_ptr<step<T> > &&next) 
        : pred(std::move(p)), done_skipping(false), next_step(std::move(next)) {}

    std::optional<T> get() override {
        if(done_skipping)
            return next_step->get();

        std::optional<T> value;
        while((value = next_step->get()) && pred(*value));

        done_skipping = true;
        return value;
    }
private:
    UnaryPred pred;
    bool done_skipping;
    std::unique_ptr<step<T> > next_step;
};


} // namespace detail



class take : public detail::step_wrapper<take> {
public:
    take(std::size_t amount) : n(amount) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        unbounded = false;
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


template<typename UnaryPred>
class take_while : public detail::step_wrapper<take_while<UnaryPred> > {
public:
    take_while(UnaryPred p) : pred(p) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        unbounded = false;
        s.reset(new detail::take_while_step<UnaryPred, T>(std::move(pred), std::move(s)));
        return std::move(st);
    }

private:
    UnaryPred pred;
};


template<typename UnaryPred>
class skip_while : public detail::step_wrapper<skip_while<UnaryPred> > {
public:
    skip_while(UnaryPred p) : pred(p) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &) {
        s.reset(new detail::skip_while_step<UnaryPred, T>(std::move(pred), std::move(s)));
        return std::move(st);
    }

private:
    UnaryPred pred;
};


static detail::reversed_t reversed;


namespace detail {
    inline void order_unused_warnings() {
        reversed();
    }
}

} // namespace streamer

#endif
