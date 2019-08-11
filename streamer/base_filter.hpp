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
class exclude : public detail::stream_manip<exclude<UnaryPred> > {
public:
    exclude(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        values.erase(std::remove_if(values.begin(), values.end(), std::move(pred)), values.end());
        return std::move(st); 
    }

private:
    UnaryPred pred;
};


template<typename UnaryPred>
class filter : public detail::stream_manip<filter<UnaryPred> > {
public:
    filter(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        return exclude([this](auto x) { return !pred(x); }).stream(st, values);
    }

private:
    UnaryPred pred;
};


template<typename UnaryPred>
class first_if_t : public detail::stream_manip<first_if_t<UnaryPred> > {
public:
    first_if_t(UnaryPred &&p) : pred(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        auto it = std::find_if(values.begin(), values.end(), std::move(pred));
        if(it == values.end())
            return {};
        return std::optional<T>(std::move(*it));
    }

private:
    UnaryPred pred;
};


static class first_t : public detail::stream_manip<first_t> {
public:
    first_t &operator()() { return *this; }

    template<typename UnaryPred>
    first_if_t<UnaryPred> operator()(UnaryPred pred) { return first_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        if(values.empty())
            return {};
        return std::optional<T>(std::move(values.front()));
    }
} first;


template<typename UnaryPred>
class last_if_t : public detail::stream_manip<last_if_t<UnaryPred> > {
public:
    last_if_t(UnaryPred &&p) : pred(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        auto it = std::find_if(values.rbegin(), values.rend(), std::move(pred));
        if(it == values.rend())
            return {};
        return std::optional<T>(std::move(*it));
    }

private:
    UnaryPred pred;
};


static class last_t : public detail::stream_manip<last_t> {
public:
    last_t &operator()() { return *this; }

    template<typename UnaryPred>
    last_if_t<UnaryPred> operator()(UnaryPred pred) { return last_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        if(values.empty())
            return {};
        return std::optional<T>(std::move(values.back()));
    }
} last;


template<typename UnaryPred>
class single_if_t : public detail::stream_manip<single_if_t<UnaryPred> > {
public:
    single_if_t(UnaryPred &&p) : pred(std::move(p)) {}

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        auto it = std::find_if(values.begin(), values.end(), pred);
        if(it == values.end())
            return {};
        if(std::find_if(it + 1, values.end(), std::move(pred)) != values.end())
            throw single_error("streamer contains multiple values matching predicate.");
        return std::optional<T>(std::move(*it));
    }

private:
    UnaryPred pred;
};


static class single_t : public detail::stream_manip<single_t> {
public:
    single_t &operator()() { return *this; }

    template<typename UnaryPred>
    single_if_t<UnaryPred> operator()(UnaryPred pred) { return single_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::optional<T> stream(streamer_t<T> &, std::vector<T> &values) {
        if(values.empty())
            return {};
        if(values.size() > 1)
            throw single_error("streamer contains " + std::to_string(values.size()) + " values. Expected 1.");
        return std::optional<T>(std::move(values.front()));
    }
} single;


namespace detail {
    inline void filter_unused_warnings() {
        first();
        last();
        single();
    }
}


} // namespace streamer

#endif