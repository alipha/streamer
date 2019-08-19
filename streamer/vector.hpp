#ifndef STREAMER_VECTOR_HPP
#define STREAMER_VECTOR_HPP

#include "base.hpp"
#include <initializer_list>
#include <vector>


namespace streamer {


namespace detail {

    
class as_vector_t : public step_wrapper<as_vector_t> {
public:
    constexpr as_vector_t &operator()() noexcept { return *this; }

    template<typename T>
    std::vector<T> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_vector on an unbounded stream");

        return std::vector<T>(st.begin(), st.end());
    }
};


}  // namespace detail


static detail::as_vector_t as_vector;



template<typename T>
streamer_t<T> stream_of(std::initializer_list<T> init) {
    return streamer_t<T>(std::vector<T>(init));
}


namespace detail {
    inline void vector_unused_warnings() {
        as_vector();
    }
} // namespace detail

} // namespace streamer

#endif
