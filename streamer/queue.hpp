#ifndef STREAMER_QUEUE_HPP
#define STREAMER_QUEUE_HPP

#include "base.hpp"
#include <queue>
#include <deque>
#include <functional>


namespace streamer {


static class as_queue_t : public detail::stream_manip<as_queue_t> {
public:
    as_queue_t &operator()() { return *this; }

    template<typename T>
    std::queue<T> stream(streamer_t<T> &, std::vector<T> &values) {
        return std::queue<T>(std::deque<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end())));
    }
} as_queue;


template<typename Comp>
class as_priority_queue_custom_t : public detail::stream_manip<as_priority_queue_custom_t<Comp> > {
public:
    as_priority_queue_custom_t(Comp &&c) : comp(std::move(c)) {}

    template<typename T>
    std::priority_queue<T, std::vector<T>, Comp> stream(streamer_t<T> &st, std::vector<T> &values) {
        return std::priority_queue(comp, std::move(values));
    }
private:
    Comp comp;
};


static class as_priority_queue_t : public detail::stream_manip<as_priority_queue_t> {
public:
    as_priority_queue_t &operator()() { return *this; }

    template<typename Comp>
    as_priority_queue_custom_t<Comp> operator()(Comp comp) {
        return as_priority_queue_custom_t<Comp>(std::move(comp));
    }

    template<typename T>
    std::priority_queue<T> stream(streamer_t<T> &st, std::vector<T> &values) {
        return as_priority_queue_custom_t(std::less<T>()).stream(st, values);
    }
} as_priority_queue;


namespace detail {
    inline void queue_unused_warnings() {
        as_queue();
        as_priority_queue();
    }
}

} // namespace streamer

#endif
