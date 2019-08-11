#ifndef STREAMER_VECTOR_HPP
#define STREAMER_VECTOR_HPP

#include "base.hpp"
#include <vector>


namespace streamer {


namespace detail {

    
class as_vector_t : public detail::step_wrapper<as_vector_t> {
public:
    as_vector_t &operator()() { return *this; }

    template<typename T>
    std::vector<T> stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_vector on an unbounded stream");

        std::vector<T> out;

        while(std::optional<T> value = s->get()) {
            out.push_back(std::move(*value));
        }
        return out;
    }
};


}  // namespace detail


static detail::as_vector_t as_vector;


namespace detail {
    inline void vector_unused_warnings() {
        as_vector();
    }
} // namespace detail

} // namespace streamer

#endif
