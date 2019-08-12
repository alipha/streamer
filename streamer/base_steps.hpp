#ifndef STREAMER_BASE_STEPS_HPP
#define STREAMER_BASE_STEPS_HPP

#include "base.hpp"


namespace streamer {


namespace detail {


template<typename MemOrFunc, typename T, typename U>
class mapping_step : public step<U> {
public:
    mapping_step(MemOrFunc &&f, std::unique_ptr<step<T> > &&next) 
        : func(std::move(f)), next_step(std::move(next)) {}

    std::optional<U> get() override {
        if(std::optional<T> value = next_step->get())
            return {func(*std::move(value))};
        else
            return {};
    }
private:
    MemOrFunc func;
    std::unique_ptr<step<T> > next_step;
};


template<typename MemOrFunc, typename Cont, typename T, typename U>
class flat_mapping_step : public step<U> {
public:
    flat_mapping_step(MemOrFunc &&f, std::unique_ptr<step<T> > &&next) 
        : func(std::move(f)), state(), next_step(std::move(next)) { set_state(); }

    std::optional<U> get() override {
        while(state->it == state->end) {
            set_state();
            if(!state)
                return {};
        }

        U value = *state->it;
        state->it++;
        return {value};
    }

private:
    struct state_t {
        state_t(Cont &&c) : cont(std::move(c)), it(std::begin(cont)), end(std::end(cont)) {}

        state_t(const state_t &) = delete;
        state_t(state_t &&) = delete;
        state_t &operator=(const state_t &) = delete;
        state_t &operator=(state_t &&) = delete;

        Cont cont;
        typename std::remove_reference<decltype(std::begin(cont))>::type it;
        typename std::remove_reference<decltype(std::begin(cont))>::type end;
    };

    void set_state() {
        std::optional<T> value = next_step->get();
        if(value)
            state.emplace(func(*std::move(value)));
        else
            state.reset();
    }

    MemOrFunc func;
    std::optional<state_t> state;
    std::unique_ptr<step<T> > next_step;
};


template<typename UnaryPred>
class count_if_t : public step_wrapper<count_if_t<UnaryPred> > {
public:
    count_if_t(UnaryPred &&p) : pred(std::move(p)) {}

    template<typename T>
    std::size_t stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use item_count(UnaryPred) on an unbounded stream");

        std::size_t i;

        for(i = 0; std::optional<T> value = s->get(); ) {
            if(pred(*std::move(value)))
                i++;
        }

        return i;
    }

private:
    UnaryPred pred;
};


class count_t : public detail::step_wrapper<count_t> {
public:
    constexpr count_t &operator()() noexcept { return *this; }

    template<typename UnaryPred>
    count_if_t<UnaryPred> operator()(UnaryPred pred) { return count_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::size_t stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use item_count on an unbounded stream");

        std::size_t i;
        for(i = 0; s->get(); i++) {}
        return i;
    }
};


} // namespace detail


template<typename MemOrFunc>
class mapping : public detail::step_wrapper<mapping<MemOrFunc> > {
public:
    mapping(MemOrFunc f) : func(std::move(f)) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        auto f = detail::member_mapper(func);
        using U = typename detail::remove_ref_cv<decltype(f(*std::move(s->get())))>::type;

        auto ptr = new detail::mapping_step<decltype(f), T, U>(std::move(f), std::move(s)); 
        std::unique_ptr<detail::step<U> > step(ptr);
        return streamer_t<U>(std::move(step), unbounded);
    }

private:
    MemOrFunc func;
};


template<typename MemOrFunc>
class flat_mapping : public detail::step_wrapper<flat_mapping<MemOrFunc> > {
public:
    flat_mapping(MemOrFunc f) : func(std::move(f)) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        auto f = detail::member_mapper(func);
        using Cont = typename detail::remove_ref_cv<decltype(f(*std::move(s->get())))>::type;
        using U = typename detail::remove_ref_cv<decltype(front(f(*std::move(s->get()))))>::type;

        auto ptr = new detail::flat_mapping_step<decltype(f), Cont, T, U>(std::move(f), std::move(s)); 
        std::unique_ptr<detail::step<U> > step(ptr);
        return streamer_t<U>(std::move(step), unbounded);
    }

private:
    template<typename Cont>
    static auto front(Cont cont) { return *std::begin(cont); }

    MemOrFunc func;
};


template<typename UnaryFunc>
class each : public detail::step_wrapper<each<UnaryFunc> > {
public:
    each(UnaryFunc f) : func(std::move(f)) {}

    template<typename T>
    void stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use each on an unbounded stream");

        while(std::optional<T> value = s->get())
            func(*std::move(value));
    }

private:
    UnaryFunc func;
};


static detail::count_t item_count;


namespace detail {
    inline void steps_unused_warnings() {
        item_count();
    }
} // namespace detail

} // namespace streamer

#endif
