#ifndef STREAMER_FORWARD_LIST_HPP
#define STREAMER_FORWARD_LIST_HPP

#include "base.hpp"
#include <forward_list>


namespace streamer {


static class as_forward_list_t : public detail::stream_manip<as_forward_list_t> {
public:
    as_forward_list_t &operator()() { return *this; }

    template<typename T>
    std::forward_list<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return std::forward_list<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));
    }
} as_forward_list;


namespace detail {
    inline void forward_list_unused_warnings() {
        as_forward_list();
    }
}

} // namespace streamer

#endif
