#ifndef STREAMER_STACK_HPP
#define STREAMER_STACK_HPP

#include "base.hpp"
#include <deque>
#include <stack>


namespace streamer {


namespace detail {

    
class as_stack_t : public step_wrapper<as_stack_t> {
public:
    constexpr as_stack_t &operator()() noexcept { return *this; }

    template<typename T>
    std::stack<T> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_stack on an unbounded stream");

        return std::stack<T>(std::deque<T>(st.begin(), st.end()));
    }
};


}  // namespace detail


static detail::as_stack_t as_stack;


namespace detail {
    inline void stack_unused_warnings() {
        as_stack();
    }
} // namespace detail

} // namespace streamer

#endif
