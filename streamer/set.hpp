#ifndef STREAMER_SET_HPP
#define STREAMER_SET_HPP

#include "base.hpp"
#include <functional>
#include <set>


namespace streamer {


class duplicate_set_key : public streamer_error {
public:
    duplicate_set_key(const std::string &what) : streamer_error(what) {}
    duplicate_set_key(const char *what) : streamer_error(what) {}
};


namespace detail {

    
template<typename Comp>
class as_set_custom_t : public detail::step_wrapper<as_set_custom_t<Comp> > {
public:
    as_set_custom_t(Comp &&c, bool throw_on_dup) 
        : comp(std::move(c)), dup_throw(throw_on_dup) {}

    template<typename T>
    std::set<T, Comp> stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_set on an unbounded stream");

        if(!dup_throw)
            return std::set<T, Comp>(st.begin(), st.end(), comp);

        std::set<T, Comp> out(comp);

        while(std::optional<T> value = s->get()) {
            if(!out.insert(*std::move(value)).second)
                throw duplicate_set_key("value already exists in set");
        }

        return out;
    }

private:
    Comp comp;
    bool dup_throw;
};


class as_set_t : public detail::step_wrapper<as_set_t> {
public:
    as_set_t(bool throw_on_dup = true) : dup_throw(throw_on_dup) {}

    as_set_t operator()(bool throw_on_dup = true) noexcept { 
        return as_set_t(throw_on_dup); 
    }

    template<typename Comp>
    auto operator()(Comp comp, bool throw_on_dup = true) { 
        return as_set_custom_t(detail::member_comparer(std::move(comp)), throw_on_dup); 
    }

    template<typename KeyFunc, typename Comp>
    auto operator()(KeyFunc keyFunc, Comp comp, bool throw_on_dup = true) { 
        return as_set_custom_t(detail::member_comparer_custom(keyFunc, std::move(comp)), throw_on_dup); 
    }

    template<typename T>
    std::set<T> stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        return as_set_custom_t<std::less<T> >(std::less<T>(), dup_throw).stream(st, s, unbounded);
    }

private:
    bool dup_throw;
};



template<typename Comp>
class as_multiset_custom_t : public detail::step_wrapper<as_multiset_custom_t<Comp> > {
public:
    as_multiset_custom_t(Comp &&c) : comp(std::move(c)) {}

    template<typename T>
    std::multiset<T, Comp> stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_multiset on an unbounded stream");

        return std::multiset<T, Comp>(st.begin(), st.end(), comp);
    }

private:
    Comp comp;
};


class as_multiset_t : public detail::step_wrapper<as_multiset_t> {
public:
    constexpr as_multiset_t &operator()() noexcept { return *this; }

    template<typename Comp>
    auto operator()(Comp comp) { 
        return as_multiset_custom_t(detail::member_comparer(std::move(comp))); 
    }

    template<typename KeyFunc, typename Comp>
    auto operator()(KeyFunc keyFunc, Comp comp) { 
        return as_multiset_custom_t(detail::member_comparer_custom(keyFunc, std::move(comp))); 
    }

    template<typename T>
    std::multiset<T> stream(streamer_t<T> &st, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        return as_multiset_custom_t<std::less<T> >(std::less<T>()).stream(st, s, unbounded);
    }
};



}  // namespace detail


static detail::as_set_t as_set;
static detail::as_multiset_t as_multiset;


namespace detail {
    inline void set_unused_warnings() {
        as_set();
        as_multiset();
    }
} // namespace detail

} // namespace streamer

#endif
