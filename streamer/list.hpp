#ifndef STREAMER_LIST_HPP
#define STREAMER_LIST_HPP

#include "base.hpp"
#include <list>


namespace streamer {


static class as_list_t : public detail::stream_manip<as_list_t> {
public:
    as_list_t &operator()() { return *this; }

    template<typename T>
    std::list<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return std::list<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));
    }
} as_list;


namespace detail {
    inline void list_unused_warnings() {
        as_list();
    }
}

} // namespace streamer

#endif
