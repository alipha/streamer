#ifndef STREAMER_MATCH_HPP
#define STREAMER_MATCH_HPP

#include "base.hpp"
#include <algorithm>


namespace streamer {


template<typename UnaryPred>
class none_pred_t : public detail::stream_manip<none_pred_t<UnaryPred> > {
public:
    none_pred_t(UnaryPred &&p) : pred(std::move(p)) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::vector<T> &values) {
        return std::find_if(values.begin(), values.end(), std::move(pred)) == values.end();
    }

private:
    UnaryPred pred;
};


static class none_match_t : public detail::stream_manip<none_match_t> {
public:
    none_match_t &operator()() { return *this; }

    template<typename UnaryPred>
    none_pred_t<UnaryPred> operator()(UnaryPred pred) { return none_pred_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::vector<T> &&stream(streamer_t<T> &, std::vector<T> &values) {
        return values.empty(); 
    }
} none_match;


template<typename UnaryPred>
class any_pred_t : public detail::stream_manip<any_pred_t<UnaryPred> > {
public:
    any_pred_t(UnaryPred &&p) : pred(std::move(p)) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::vector<T> &values) {
        return !none(std::move(pred)).stream(st, values);
    }

private:
    UnaryPred pred;
};


static class any_match_t : public detail::stream_manip<any_match_t> {
public:
    any_match_t &operator()() { return *this; }

    template<typename UnaryPred>
    any_pred_t<UnaryPred> operator()(UnaryPred pred) { return any_pred_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::vector<T> &&stream(streamer_t<T> &, std::vector<T> &values) {
        return !values.empty(); 
    }
} any_match;


template<typename UnaryPred>
class all_match : public detail::stream_manip<all_match<UnaryPred> > {
public:
    all_match(UnaryPred &&p) : pred(std::move(p)) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::vector<T> &values) {
        return none([this](auto x) { return !pred(x); }).stream(st, values);
    }

private:
    UnaryPred pred;
};


namespace detail {
    inline void match_unused_warnings() {
        none_match();
        any_match();
    }
}


} // namespace streamer

#endif
