#ifndef STREAMER_LIST_HPP
#define STREAMER_LIST_HPP

#include "base.hpp"
#include <list>


namespace streamer {


namespace detail {

    
class as_list_t : public step_wrapper<as_list_t> {
public:
    constexpr as_list_t &operator()() noexcept { return *this; }

    template<typename T>
    std::list<T> stream(streamer_t<T> &st, std::unique_ptr<step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_list on an unbounded stream");

        return std::list<T>(st.begin(), st.end());
    }
};


}  // namespace detail


static detail::as_list_t as_list;


namespace detail {
    inline void list_unused_warnings() {
        as_list();
    }
} // namespace detail

} // namespace streamer

#endif
