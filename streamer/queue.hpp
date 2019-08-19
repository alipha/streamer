#ifndef STREAMER_QUEUE_HPP
#define STREAMER_QUEUE_HPP

#include "base.hpp"
#include <deque>
#include <queue>


namespace streamer {


namespace detail {

    
class as_queue_t : public step_wrapper<as_queue_t> {
public:
    constexpr as_queue_t &operator()() noexcept { return *this; }

    template<typename T>
    std::queue<T> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_queue on an unbounded stream");

        return std::queue<T>(std::deque<T>(st.begin(), st.end()));
    }
};


template<typename Comp>
class as_priority_queue_custom_t : public step_wrapper<as_priority_queue_custom_t<Comp> > {
public:
    as_priority_queue_custom_t(Comp &&c) : comp(std::move(c)) {}

    template<typename T>
    std::priority_queue<T, Comp> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_priority_queue on an unbounded stream");

        return std::priority_queue<T, Comp>(st.begin(), st.end(), comp);
    }

private:
    Comp comp;
};


class as_priority_queue_t : public step_wrapper<as_priority_queue_t> {
public:
    constexpr as_priority_queue_t &operator()() noexcept { return *this; }

    template<typename Comp>
    as_priority_queue_custom_t<Comp> operator()(Comp comp) {
        return as_priority_queue_custom_t(member_comparer(std::move(comp)));
    }

    template<typename KeyFunc, typename Comp>
    auto operator()(KeyFunc keyFunc, Comp comp) { 
        return as_priority_queue_custom_t(member_comparer_custom(keyFunc, std::move(comp))); 
    }

    template<typename T>
    std::priority_queue<T> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_priority_queue on an unbounded stream");

        return std::priority_queue<T>(std::deque<T>(st.begin(), st.end()));
    }
};


}  // namespace detail


static detail::as_queue_t as_queue;
static detail::as_priority_queue_t as_priority_queue;


namespace detail {
    inline void queue_unused_warnings() {
        as_queue();
        as_priority_queue();
    }
} // namespace detail

} // namespace streamer

#endif
