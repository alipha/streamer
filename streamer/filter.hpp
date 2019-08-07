#ifndef STREAMER_FILTER_HPP
#define STREAMER_FILTER_HPP

#include "base.hpp"
#include <algorithm>
#include <optional>


namespace streamer {


class single_error : public streamer_error {
public:
    single_error(const std::string &what) : streamer_error(what) {}
    single_error(const char *what) : streamer_error(what) {}
};


template<typename UnaryPred>
class exclude {
public:
    exclude(UnaryPred p) : pred(p) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        values.erase(std::remove_if(values.begin(), values.end(), pred), values.end());
        return std::move(st); 
    }

private:
    UnaryPred pred;
};


template<typename UnaryPred>
class filter {
public:
    filter(UnaryPred p) : pred(p) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        return exclude([this](auto x) { return !pred(x); }).stream(st, values);
    }

private:
    UnaryPred pred;
};


template<typename UnaryPred>
class first_if_t {
public:
    first_if_t(UnaryPred p) : pred(p) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        auto it = std::find_if(values.begin(), values.end(), pred);
        if(it == values.end())
            return {};
        return std::optional<T>(std::move(*it));
    }

private:
    UnaryPred pred;
};


static class first_t {
public:
    first_t &operator()() { return *this; }

    template<typename UnaryPred>
    first_if_t<UnaryPred> operator()(UnaryPred pred) { return first_if_t<UnaryPred>(pred); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        if(values.empty())
            return {};
        return std::optional<T>(std::move(*values.begin()));
    }
} first;


template<typename UnaryPred>
class last_if_t {
public:
    last_if_t(UnaryPred p) : pred(p) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        auto it = std::find_if(values.rbegin(), values.rend(), pred);
        if(it == values.rend())
            return {};
        return std::optional<T>(std::move(*it));
    }

private:
    UnaryPred pred;
};


static class last_t {
public:
    last_t &operator()() { return *this; }

    template<typename UnaryPred>
    last_if_t<UnaryPred> operator()(UnaryPred pred) { return last_if_t<UnaryPred>(pred); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        if(values.empty())
            return {};
        return std::optional<T>(std::move(*values.rbegin()));
    }
} last;


template<typename UnaryPred>
class single_if_t {
public:
    single_if_t(UnaryPred p) : pred(p) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        auto it = std::find_if(values.begin(), values.end(), pred);
        if(it == values.end())
            return {};
        if(std::find_if(it + 1, values.end(), pred) != values.end())
            throw single_error("streamer contains multiple values matching predicate.");
        return std::optional<T>(std::move(*it));
    }

private:
    UnaryPred pred;
};


static class single_t {
public:
    single_t &operator()() { return *this; }

    template<typename UnaryPred>
    single_if_t<UnaryPred> operator()(UnaryPred pred) { return single_if_t<UnaryPred>(pred); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        if(values.empty())
            return {};
        if(values.size() > 1)
            throw single_error("streamer contains " + std::to_string(values.size()) + " values. Expected 1.");
        return std::optional<T>(std::move(*values.begin()));
    }
} single;


template<typename Comp>
class distinct_custom_t {
public:
    distinct_custom_t(Comp c) : comp(c) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        auto ptrComp = [this](const T *left, const T *right) { return ptrComp(*left, *right); }
        std::set<T*, decltype(ptrComp)> unique_values;

        values.erase(std::remove_if(values.begin(), values.end(), 
            [&unique_values](auto &x) { return !unique_values.insert(&x).second; }), values.end());
        return std::move(st); 
    }

private:
    Comp comp;
};


static class distinct_t {
public:
    distinct_t &operator()() { return *this; }

    template<typename Comp>
    distinct_custom_t<Comp> operator()(Comp comp) { return distinct_custom_t<Comp>(comp); }

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        return distinct_custom_t(std::less<T>()).stream(st, values);
    }
} distinct;


namespace detail {
    inline void filter_unused_warnings() {
        first();
        last();
        single();
        distinct();
    }
}


} // namespace streamer

#endif
