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


template<typename KeyFunc, typename ValueFunc>
class as_map_t {
public:
    as_map_t(KeyFunc keyFunc, ValueFunc valueFunc, bool throw_on_dup) 
        : k(keyFunc), v(valueFunc), dup_throw(throw_on_dup) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::vector<T> &values) {
        auto k_mapper = detail::member_mapper(k);
        auto v_mapper = detail::member_mapper(v);
        using K = typename detail::remove_ref_cv<decltype(k_mapper(values[0]))>::type;
        using V = typename detail::remove_ref_cv<decltype(v_mapper(values[0]))>::type;

        std::map<K, V> out;

        for(auto &value : values) {
            K key = k_mapper(value);
            auto pair = std::make_pair(std::move(key), v_mapper(std::move(value)));
            if(!out.insert(pair).second && dup_throw)
                throw duplicate_map_key("key already exists in map");;
        }

        return out;
    }

private:
    KeyFunc k;
    ValueFunc v;
    bool dup_throw;
};


template<typename KeyFunc>
class as_map_identity_t {
public:
    as_map_identity_t(KeyFunc keyFunc, bool throw_on_dup) 
        : k(keyFunc), dup_throw(throw_on_dup) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::vector<T> &values) {
        return as_map_t(k, identity<T>(), dup_throw).stream(st, values);
    }

private:
    KeyFunc k;
    bool dup_throw;
};


template<typename KeyFunc, typename ValueFunc>
auto as_map(KeyFunc k, ValueFunc v, bool throw_on_dup = true) {
    return as_map_t(k, v, throw_on_dup);
}


template<typename KeyFunc>
auto as_map(KeyFunc k, bool throw_on_dup = true) {
    return as_map_identity_t(k, throw_on_dup);
}



template<typename KeyFunc, typename ValueFunc>
class as_multimap_t {
public:
    as_multimap_t(KeyFunc keyFunc, ValueFunc valueFunc) : k(keyFunc), v(valueFunc) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::vector<T> &values) {
        auto k_mapper = detail::member_mapper(k);
        auto v_mapper = detail::member_mapper(v);
        using K = typename detail::remove_ref_cv<decltype(k_mapper(values[0]))>::type;
        using V = typename detail::remove_ref_cv<decltype(v_mapper(values[0]))>::type;

        std::multimap<K, V> out;

        for(auto &value : values) {
            K key = k_mapper(value);
            out.insert(std::move(key), v_mapper(std::move(value)));
        }

        return out;
    }

private:
    KeyFunc k;
    ValueFunc v;
};


template<typename KeyFunc>
class as_multimap_identity_t {
public:
    as_multimap_identity_t(KeyFunc keyFunc) : k(keyFunc) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::vector<T> &values) {
        return as_multimap_t(k, identity<T>()).stream(st, values);
    }

private:
    KeyFunc k;
};


template<typename KeyFunc, typename ValueFunc>
auto as_multimap(KeyFunc k, ValueFunc v) {
    return as_multimap_t(k, v);
}


template<typename KeyFunc>
auto as_multimap(KeyFunc k) {
    return as_multimap_identity_t(k);
}



template<typename KeyFunc, typename ValueFunc>
class as_grouping_t {
public:
    as_grouping_t(KeyFunc keyFunc, ValueFunc valueFunc) : k(keyFunc), v(valueFunc) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::vector<T> &values) {
        auto k_mapper = detail::member_mapper(k);
        auto v_mapper = detail::member_mapper(v);
        using K = typename detail::remove_ref_cv<decltype(k_mapper(values[0]))>::type;
        using V = typename detail::remove_ref_cv<decltype(v_mapper(values[0]))>::type;

        std::map<K, std::vector<V>> out;

        for(auto &value : values) {
            K key = k_mapper(value);
            out[std::move(key)].push_back(v_mapper(std::move(value)));
        }

        return out;
    }

private:
    KeyFunc k;
    ValueFunc v;
};


template<typename KeyFunc>
class as_grouping_identity_t {
public:
    as_grouping_identity_t(KeyFunc keyFunc) 
        : k(keyFunc) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::vector<T> &values) {
        return as_grouping_t(k, identity<T>()).stream(st, values);
    }

private:
    KeyFunc k;
};


template<typename KeyFunc, typename ValueFunc>
auto as_grouping(KeyFunc k, ValueFunc v) {
    return as_grouping_t(k, v);
}


template<typename KeyFunc>
auto as_grouping(KeyFunc k) {
    return as_grouping_identity_t(k);
}


} // namespace streamer

#endif