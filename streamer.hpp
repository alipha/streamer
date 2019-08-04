#ifndef STREAMER_HPP
#define STREAMER_HPP

#include "streamer_helpers.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <sstream>
#include <type_traits>
#include <vector>
#include <utility>



template<typename T>
class streamer_t {
public:
    typedef typename std::vector<T>::value_type value_type;


    streamer_t() noexcept : values() {}

    explicit streamer_t(std::vector<T> &&v) noexcept : values(move(v)) {}

    template<typename It>
    streamer_t(It begin, It end) : values(begin, end) {}


    streamer_t(streamer_t &&) = default;
    streamer_t &operator=(streamer_t &&) = default;
    
    streamer_t(const streamer_t &) = delete;
    streamer_t &operator=(const streamer_t &) = delete;


    template<typename UnaryFunc>
    auto map(UnaryFunc &&f) && {
        auto mapper = member_mapper(f);
        using U = typename std::remove_reference<decltype(mapper(values[0]))>::type; 
        std::vector<U> out;
        std::transform(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()), std::back_inserter(out), mapper);
        return streamer_t<U>(std::move(out));
    }


    template<typename UnaryFunc>
    auto flat_map(UnaryFunc &&f) && {
        using Cont = typename std::remove_reference<decltype(f(values[0]))>::type;
        using U = typename Cont::value_type;
        
        std::vector<U> out;

        for(auto &value : values) {
            auto items = f(std::move(value));
            auto range = get_range(items);
            out.insert(out.end(), range.first, range.second);
        }

        return streamer_t<U>(std::move(out));
    }


    template<typename UnaryPred>
    streamer_t&& filter(UnaryPred &&pred) && {
        return std::move(*this).exclude([pred](auto x) { return !pred(x); });
    }

    template<typename UnaryPred>
    streamer_t&& exclude(UnaryPred &&pred) && {
        values.erase(std::remove_if(values.begin(), values.end(), pred), values.end());
        return std::move(*this);
    }


    streamer_t&& reverse() && {
        std::reverse(values.begin(), values.end());
        return std::move(*this);
    }


    std::size_t count() && { return values.size(); }

    template<typename UnaryPred>
    std::size_t count(UnaryPred &&pred) && {
        return std::count_if(values.begin(), values.end(), pred);
    }


    bool any() && { return !values.empty(); }

    template<typename UnaryPred>
    bool any(UnaryPred &&pred) && {
        return !std::move(*this).none(pred);
    }


    bool none() && { return values.empty(); }

    template<typename UnaryPred>
    bool none(UnaryPred &&pred) && {
        return std::find_if(values.begin(), values.end(), pred) == values.end();
    }


    template<typename UnaryPred>
    bool all(UnaryPred &&pred) && {
        return std::move(*this).none([pred](auto x) { return !pred(x); });
    }


    std::optional<T> first() && {
        if(values.empty())
            return {};
        return std::optional<T>(std::move(*values.begin()));
    }

    template<typename UnaryPred>
    std::optional<T> first(UnaryPred &&pred) && {
        auto it = std::find_if(values.begin(), values.end(), pred);
        if(it == values.end())
            return {};
        return std::optional<T>(std::move(*it));
    }


    std::optional<T> single() && {
        if(values.empty())
            return {};
        if(values.size() > 1)
            throw single_error("streamer contains " + std::to_string(values.size()) + " values. Expected 1.");
        return std::optional<T>(std::move(*values.begin()));
    }

    template<typename UnaryPred>
    std::optional<T> single(UnaryPred &&pred) && {
        auto it = std::find_if(values.begin(), values.end(), pred);
        if(it == values.end())
            return {};
        if(std::find_if(it + 1, values.end(), pred) != values.end())
            throw single_error("streamer contains multiple values matching predicate.");
        return std::optional<T>(std::move(*it));
    }


    std::optional<T> last() && {
        if(values.empty())
            return {};
        return std::optional<T>(std::move(*values.rbegin()));
    }

    template<typename UnaryPred>
    std::optional<T> last(UnaryPred &&pred) && {
        auto it = std::find_if(values.rbegin(), values.rend(), pred);
        if(it == values.rend())
            return {};
        return std::optional<T>(std::move(*it));
    }


    streamer_t&& take(std::size_t n) && {
        if(n > values.size())
            values.resize(n);
        return std::move(*this);
    }

    streamer_t&& skip(std::size_t n) && {
        if(n < values.size())
            values.erase(values.begin(), values.begin() + n);
        else
            values.clear();
        return std::move(*this);
    }


    streamer_t&& sort() && {
        return std::move(*this).sort(std::less<T>());
    }

    template<typename Comp>
    streamer_t&& sort(Comp &&comp) && {
        std::sort(values.begin(), values.end(), comp);
        return std::move(*this);
    }

    template<typename KeyFunc, typename Comp>
    streamer_t&& sort(KeyFunc k, Comp &&comp) && {
        return std::move(*this).sort(member_comparer(k, comp));
    }


    template<typename UnaryFunc>
    streamer_t&& for_each(UnaryFunc &&f) && {
        std::for_each(values.begin(), values.end(), f);
        return std::move(*this);
    }


    template<typename Char>
    std::basic_string<Char> join(const Char *delim) {
        return std::move(*this).join(std::basic_string<Char>(delim));
    }

    template<typename Char>
    std::basic_string<Char> join(const std::basic_string<Char> &delim) {
        if(values.empty())
            return {};
        std::basic_stringstream<Char> out;

        out << *values.begin();

        for(auto it = values.begin() + 1; it != values.end(); ++it)
            out << delim << *it;

        return out.str();
    }


    std::vector<T> as_vector() && noexcept {
        return values;
    }


    std::set<T> as_set(bool throw_on_dup = true) && {
        return std::move(*this).as_set(std::less<T>(), throw_on_dup);
    }

    template<typename Comp>
    std::set<T, Comp> as_set(Comp &&comp, bool throw_on_dup = true) && {
        if(!throw_on_dup)
            return std::set<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));

        std::set<T, Comp> out;

        for(auto &value : values) {
            if(!out.insert(std::move(value))->second && throw_on_dup)
                throw duplicate_set_key("value already exists in set");
        }

        return out;
    };


    std::multiset<T> as_multiset() && {
        return std::move(*this).as_multiset(std::less<T>());
    }

    template<typename Comp>
    std::multiset<T, Comp> as_multiset(Comp &&comp) && {
        return std::multiset<T>(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()), comp);
    }


    template<typename KeyFunc>
    auto as_map(KeyFunc k, bool throw_on_dup = true) && {
        return std::move(*this).as_map(k, identity<T>(), throw_on_dup);
    }

    template<typename KeyFunc, typename ValueFunc>
    auto as_map(KeyFunc k, ValueFunc v, bool throw_on_dup = true) && {
        auto k_mapper = member_mapper(k);
        auto v_mapper = member_mapper(v);
        using K = typename std::remove_reference<decltype(k_mapper(values[0]))>::type;
        using V = typename std::remove_reference<decltype(v_mapper(values[0]))>::type;

        std::map<K, V> out;

        for(auto &value : values) {
            K key = k_mapper(value);
            auto pair = std::make_pair(std::move(key), v_mapper(std::move(value)));
            if(!out.insert(pair).second && throw_on_dup)
                throw duplicate_map_key("key already exists in map");;
        }

        return out;
    }
    /* TODO: as_map, as_multimap, and as_listmap with KeyComp params */
    /* TODO: add as_unordered_set, as_unordered_multiset,
     *       as_unordered_map, as unordered_multimap, as_unordered_listmap,
     *       as_deque, as_list, as_queue, as_stack,
     *       fill, fill_map, fill_listmap
     */

    template<typename KeyFunc>
    auto as_multimap(KeyFunc k) && {
        return as_multimap(k, identity<T>());
    }

    template<typename KeyFunc, typename ValueFunc>
    auto as_multimap(KeyFunc k, ValueFunc v) && {
        auto k_mapper = member_mapper(k);
        auto v_mapper = member_mapper(v);
        using K = typename std::remove_reference<decltype(k_mapper(values[0]))>::type;
        using V = typename std::remove_reference<decltype(v_mapper(values[0]))>::type;

        std::multimap<K, V> out;

        for(auto &value : values) {
            K key = k_mapper(value);
            out.insert(std::move(key), v_mapper(std::move(value)));
        }

        return out;
    }


    template<typename KeyFunc>
    auto group_by(KeyFunc k) && {
        return std::move(*this).group_by(k, identity<T>());
    }

    template<typename KeyFunc, typename ValueFunc>
    auto group_by(KeyFunc k, ValueFunc v) && {
        auto k_mapper = member_mapper(k);
        auto v_mapper = member_mapper(v);
        using K = typename std::remove_reference<decltype(k_mapper(values[0]))>::type;
        using V = typename std::remove_reference<decltype(v_mapper(values[0]))>::type;
 
        std::map<K, std::vector<V>> out;

        for(auto &value : values) {
            K key = k_mapper(value);
            out[std::move(key)].push_back(v_mapper(std::move(value)));
        }

        return out;
    }

private:
    template<class U>
    friend class streamer_t;


    template<typename Cont>
    static auto get_range(const Cont &c) {
        return std::make_pair(c.begin(), c.end());
    }

    template<typename U>
    static auto get_range(const streamer_t<U> &s) {
        return std::make_pair(s.values.begin(), s.values.end());
    }

 
    std::vector<T> values;
};



template<typename T>
streamer_t<T> streamer(const T *p, std::size_t n) {
    return streamer_t<T>(p, p + n);
}

template<typename T, std::size_t N>
streamer_t<T> streamer(const T (&a)[N]) {
    return streamer_t<T>(a, a + N);
}

template<typename Cont>
streamer_t<typename Cont::value_type> streamer(const Cont &c) {
    return streamer_t<typename Cont::value_type>(c.begin(), c.end());
}

template<typename T>
streamer_t<T> streamer(std::vector<T> &&v) noexcept {
    return streamer_t<T>(std::move(v));
}

template<typename T>
streamer_t<T> streamer_of(std::initializer_list<T> init) noexcept {
    return streamer_t<T>(std::vector<T>(init));
}

template<typename It>
streamer_t<typename std::iterator_traits<It>::value_type> streamer(It begin, It end) {
    return streamer_t<typename std::iterator_traits<It>::value_type>(begin, end);
}


#endif
