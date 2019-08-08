#ifndef STREAMER_DEQUE_HPP
#define STREAMER_DEQUE_HPP

#include "base.hpp"
#include <deque>


namespace streamer {


static class as_deque_t : public detail::stream_manip<as_deque_t> {
public:
    as_deque_t &operator()() { return *this; }

    template<typename T>
    std::deque<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return std::deque<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));
    }
} as_deque;


namespace detail {
    inline void deque_unused_warnings() {
        as_deque();
    }
}

} // namespace streamer

#endif
