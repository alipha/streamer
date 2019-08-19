#ifndef STREAMER_BASE_MATCH_HPP
#define STREAMER_BASE_MATCH_HPP

#include "base.hpp"
#include "base_filter.hpp"


namespace streamer {

    
namespace detail {

    
template<typename UnaryPred>
class any_match_if_t : public step_wrapper<any_match_if_t<UnaryPred> > {
public:
    any_match_if_t(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    bool stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use any_match(UnaryPred) on an unbounded stream");
            
        return filter_step<UnaryPred, T>(std::move(pred), std::move(s)).get();
    }

private:
    UnaryPred pred;
};


class any_match_t : public step_wrapper<any_match_t> {
public:
    constexpr any_match_t &operator()() noexcept { return *this; }

    template<typename UnaryPred>
    any_match_if_t<UnaryPred> operator()(UnaryPred pred) { return any_match_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    bool stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &) {
        return s->get();
    }
};


template<typename UnaryPred>
class none_match_if_t : public step_wrapper<none_match_if_t<UnaryPred> > {
public:
    none_match_if_t(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use none_match(UnaryPred) on an unbounded stream");

        return !any_match_t<UnaryPred>(std::move(pred)).stream(st, s, unbounded);
    }

private:
    UnaryPred pred;
};


class none_match_t : public step_wrapper<none_match_t> {
public:
    constexpr none_match_t &operator()() noexcept { return *this; }

    template<typename UnaryPred>
    none_match_if_t<UnaryPred> operator()(UnaryPred pred) { return none_match_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    bool stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &) {
        return !s->get();
    }
};



} // namespace detail


template<typename UnaryPred>
class all_match : public detail::step_wrapper<all_match<UnaryPred> > {
public:
    all_match(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use all_match on an unbounded stream");

        return none_match_t<UnaryPred>([this](auto x) { return !pred(x); }).stream(st, s, unbounded);
    }

private:
    UnaryPred pred;
};


static detail::any_match_t any_match;
static detail::none_match_t none_match;


namespace detail {
    inline void match_unused_warnings() {
        any_match();
        none_match();
    }
} // namespace detail


} // namespace streamer

#endif
