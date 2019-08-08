#ifndef STREAMER_STACK_HPP
#define STREAMER_STACK_HPP

#include "base.hpp"
#include <stack>
#include <deque>


namespace streamer {


static class as_stack_t : public detail::stream_manip<as_stack_t> {
public:
    as_stack_t &operator()() { return *this; }

    template<typename T>
    std::stack<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return std::stack<T>(std::deque<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
    }
} as_stack;


namespace detail {
    inline void stack_unused_warnings() {
        as_stack();
    }
}

} // namespace streamer

#endif
