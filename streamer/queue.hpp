#ifndef STREAMER_QUEUE_HPP
#define STREAMER_QUEUE_HPP

#include "base.hpp"
#include <deque>
#include <queue>


namespace streamer {


namespace detail {

    
class as_queue_t : public detail::step_wrapper<as_queue_t> {
public:
    constexpr as_queue_t &operator()() noexcept { return *this; }

    template<typename T>
    std::queue<T> stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_queue on an unbounded stream");

        return std::queue<T>(std::deque<T>(st.begin(), st.end()));
    }
};


}  // namespace detail


static detail::as_queue_t as_queue;


namespace detail {
    inline void queue_unused_warnings() {
        as_queue();
    }
} // namespace detail

} // namespace streamer

#endif
