#ifndef STREAMER_DEQUE_HPP
#define STREAMER_DEQUE_HPP

#include "base.hpp"
#include <deque>


namespace streamer {


namespace detail {

    
class as_deque_t : public step_wrapper<as_deque_t> {
public:
    constexpr as_deque_t &operator()() noexcept { return *this; }

    template<typename T>
    std::deque<T> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_deque on an unbounded stream");

        return std::deque<T>(st.begin(), st.end());
    }
};


}  // namespace detail


static detail::as_deque_t as_deque;


namespace detail {
    inline void deque_unused_warnings() {
        as_deque();
    }
} // namespace detail

} // namespace streamer

#endif
