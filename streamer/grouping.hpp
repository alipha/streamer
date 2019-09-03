#ifndef STREAMER_GROUPING_HPP
#define STREAMER_GROUPING_HPP

#include "base.hpp"
#include <map>
#include <vector>


namespace streamer {


template<typename KeyFunc, typename ValueFunc, typename Comp>
class as_grouping_t : public step_wrapper<as_grouping_t<KeyFunc, ValueFunc, Comp> > {
public:
    as_grouping_t(KeyFunc &&keyFunc, ValueFunc &&valueFunc, Comp &&comp)
        : k(std::move(keyFunc)), v(std::move(valueFunc)), c(std::move(comp)) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::unique_ptr<step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use as_grouping on an unbounded stream");

        using K = typename remove_ref_cv<decltype(k(*s->get()))>::type;
        using V = typename remove_ref_cv<decltype(v(*std::move(s->get())))>::type;

        std::map<K, std::vector<V>, Comp> out(c);

        while(std::optional<T> value = s->get()) {
            K key = k(*value);
            out[std::move(key)].push_back(v(*std::move(value)));
        }

        return out;
    }

private:
    KeyFunc k;
    ValueFunc v;
    Comp c;
};


template<typename KeyFunc, typename ValueFunc>
class as_grouping_less_t : public step_wrapper<as_grouping_less_t<KeyFunc, ValueFunc> > {
public:
    as_grouping_less_t(KeyFunc &&keyFunc, ValueFunc &&valueFunc)
        : k(std::move(keyFunc)), v(std::move(valueFunc)) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::unique_ptr<step<T> > &s, bool &unbounded) {
        using K = typename remove_ref_cv<decltype(k(*s->get()))>::type;
        return as_grouping_t(std::move(k), std::move(v), std::less<K>()).stream(st, s, unbounded);
    }

private:
    KeyFunc k;
    ValueFunc v;
};


template<typename KeyFunc>
class as_grouping_identity_t : public step_wrapper<as_grouping_identity_t<KeyFunc> > {
public:
    as_grouping_identity_t(KeyFunc &&keyFunc) : k(std::move(keyFunc)) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::unique_ptr<step<T> > &s, bool &unbounded) {
        using K = typename remove_ref_cv<decltype(k(*s->get()))>::type;
        return as_grouping_t(std::move(k), identity<T>(), std::less<K>()).stream(st, s, unbounded);
    }

private:
    KeyFunc k;
};


}  // namespace detail


template<typename KeyFunc, typename ValueFunc, typename Comp>
auto as_grouping(KeyFunc k, ValueFunc v, Comp c) {
    return detail::as_grouping_t(
        detail::member_mapper(std::move(k)), 
        detail::member_mapper(std::move(v)), 
        std::move(c) 
    );
}


template<typename KeyFunc, typename ValueFunc>
auto as_grouping(KeyFunc k, ValueFunc v) {
    return detail::as_grouping_less_t(
        detail::member_mapper(std::move(k)), 
        detail::member_mapper(std::move(v))
   );
}


template<typename KeyFunc>
auto as_grouping(KeyFunc k) {
    return detail::as_grouping_identity_t(detail::member_mapper(std::move(k)));
}


} // namespace streamer

#endif

