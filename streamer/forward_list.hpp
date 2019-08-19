#ifndef STREAMER_FORWARD_LIST_HPP
#define STREAMER_FORWARD_LIST_HPP

#include "base.hpp"
#include <forward_list>


namespace streamer {


namespace detail {

    
class as_forward_list_t : public step_wrapper<as_forward_list_t> {
public:
    constexpr as_forward_list_t &operator()() noexcept { return *this; }

    template<typename T>
    std::forward_list<T> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_forward_list on an unbounded stream");

        return std::forward_list<T>(st.begin(), st.end());
    }
};


}  // namespace detail


static detail::as_forward_list_t as_forward_list;


namespace detail {
    inline void forward_list_unused_warnings() {
        as_forward_list();
    }
} // namespace detail

} // namespace streamer

#endif
