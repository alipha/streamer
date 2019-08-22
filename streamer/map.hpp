#ifndef STREAMER_MAP_HPP
#define STREAMER_MAP_HPP

#include "base.hpp"
#include <map>


namespace streamer {


class duplicate_map_key : public streamer_error {
public:
    duplicate_map_key(const std::string &what) : streamer_error(what) {}
    duplicate_map_key(const char *what) : streamer_error(what) {}
};


namespace detail {


template<typename KeyFunc, typename ValueFunc, typename Comp>
class as_map_t : public step_wrapper<as_map_t<KeyFunc, ValueFunc, Comp> > {
public:
    as_map_t(KeyFunc &&keyFunc, ValueFunc &&valueFunc, Comp &&comp, bool throw_on_dup)
        : k(std::move(keyFunc)), v(std::move(valueFunc)), c(std::move(comp)), dup_throw(throw_on_dup) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_map on an unbounded stream");

        //auto k_mapper = member_mapper(k);
        //auto v_mapper = member_mapper(v);
        using K = typename remove_ref_cv<decltype(k(*s->get()))>::type;
        using V = typename remove_ref_cv<decltype(v(*std::move(s->get())))>::type;

        std::map<K, V, Comp> out(c);

        while(std::optional<T> value = s->get()) {
            K key = k(*value);
            auto pair = std::make_pair(std::move(key), v(*std::move(value)));
            if(!out.insert(std::move(pair)).second && dup_throw)
                throw duplicate_map_key("key already exists in map");;
        }

        return out;
    }

private:
    KeyFunc k;
    ValueFunc v;
    Comp c;
    bool dup_throw;
};


template<typename KeyFunc, typename ValueFunc>
class as_map_less_t : public step_wrapper<as_map_less_t<KeyFunc, ValueFunc> > {
public:
    as_map_less_t(KeyFunc &&keyFunc, ValueFunc &&valueFunc, bool throw_on_dup)
        : k(std::move(keyFunc)), v(std::move(valueFunc)), dup_throw(throw_on_dup) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::unique_ptr<step<T> > &s, bool &unbounded) {
        using K = typename remove_ref_cv<decltype(k(*s->get()))>::type;
        return as_map_t(std::move(k), std::move(v), std::less<K>(), dup_throw).stream(st, s, unbounded);
    }

private:
    KeyFunc k;
    ValueFunc v;
    bool dup_throw;
};


template<typename KeyFunc>
class as_map_identity_t : public step_wrapper<as_map_identity_t<KeyFunc> > {
public:
    as_map_identity_t(KeyFunc &&keyFunc, bool throw_on_dup)
        : k(std::move(keyFunc)), dup_throw(throw_on_dup) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::unique_ptr<step<T> > &s, bool &unbounded) {
        using K = typename remove_ref_cv<decltype(k(*s->get()))>::type;
        return as_map_t(std::move(k), identity<T>(), std::less<K>(), dup_throw).stream(st, s, unbounded);
    }

private:
    KeyFunc k;
    bool dup_throw;
};


}  // namespace detail


template<typename KeyFunc, typename ValueFunc, typename Comp>
auto as_map(KeyFunc k, ValueFunc v, Comp c, bool throw_on_dup = true) {
    return detail::as_map_t(
        detail::member_mapper(std::move(k)), 
        detail::member_mapper(std::move(v)), 
        std::move(c), 
        throw_on_dup
    );
}


template<typename KeyFunc, typename ValueFunc>
auto as_map(KeyFunc k, ValueFunc v, bool throw_on_dup = true) {
    return detail::as_map_less_t(
        detail::member_mapper(std::move(k)), 
        detail::member_mapper(std::move(v)),
        throw_on_dup
   );
}


template<typename KeyFunc>
auto as_map(KeyFunc k, bool throw_on_dup = true) {
    return detail::as_map_identity_t(detail::member_mapper(std::move(k)), throw_on_dup);
}


} // namespace streamer

#endif
