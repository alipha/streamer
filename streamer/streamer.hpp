#ifndef STREAMER_STREAMER_HPP
#define STREAMER_STREAMER_HPP

#include "base.hpp"
#include <algorithm>
#include <iterator>


namespace streamer {

    
template<typename UnaryFunc>
class mapper {
public:
    mapper(UnaryFunc f) : func(f) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::vector<T> &values) {
        auto f = detail::member_mapper(func);
        using U = typename detail::remove_ref_cv<decltype(f(values[0]))>::type;

        std::vector<U> out;
        std::transform(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()), std::back_inserter(out), f);
        return streamer_t<U>(std::move(out));
    }
private:
    UnaryFunc func;
};


template<typename UnaryFunc>
class flat_map {
public:
    flat_map(UnaryFunc f) : func(f) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::vector<T> &values) {
        //using Cont = typename detail::remove_ref_cv<decltype(func(std::move(values[0])))>::type;
        using U = typename detail::remove_ref_cv<decltype(detail::range(func(std::move(values[0]))))>::type;
        
        std::vector<U> out;

        for(auto &value : values) {
            auto items = func(std::move(value));
            auto r = detail::range(items);
            out.insert(out.end(), r.first, r.second);
        }

        return streamer_t<U>(std::move(out));
    }

private:
    UnaryFunc func;
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


static class backwards_t {
public:
    backwards_t &operator()() { return *this; }

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        std::reverse(values.begin(), values.end());
        return std::move(st); 
    }
} backwards;


template<typename UnaryPred>
class count_if_t {
public:
    count_if_t(UnaryPred p) : pred(p) {}

    template<typename T>
    std::size_t stream(streamer_t<T> &, std::vector<T> &values) {
        return std::count_if(values.begin(), values.end(), pred);
    }

private:
    UnaryPred pred;
};


static class item_count_t {
public:
    item_count_t &operator()() { return *this; }

    template<typename UnaryPred>
    count_if_t<UnaryPred> operator()(UnaryPred pred) { return count_if_t<UnaryPred>(pred); }

    template<typename T>
    std::vector<T> &&stream(streamer_t<T> &, std::vector<T> &values) {
        return values.size(); 
    }
} item_count;


static class as_vector_t {
public:
    as_vector_t &operator()() { return *this; }

    template<typename T>
    std::vector<T> &&stream(streamer_t<T> &, std::vector<T> &values) {
        return std::move(values); 
    }
} as_vector;


template<typename UnaryPred>
class none_pred_t {
public:
    none_pred_t(UnaryPred p) : pred(p) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::vector<T> &values) {
        return std::find_if(values.begin(), values.end(), pred) == values.end();
    }

private:
    UnaryPred pred;
};


static class none_match_t {
public:
    none_match_t &operator()() { return *this; }

    template<typename UnaryPred>
    none_pred_t<UnaryPred> operator()(UnaryPred pred) { return none_pred_t<UnaryPred>(pred); }

    template<typename T>
    std::vector<T> &&stream(streamer_t<T> &, std::vector<T> &values) {
        return values.empty(); 
    }
} none_match;


template<typename UnaryPred>
class any_pred_t {
public:
    any_pred_t(UnaryPred p) : pred(p) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::vector<T> &values) {
        return !none(pred).stream(st, values);
    }

private:
    UnaryPred pred;
};


static class any_match_t {
public:
    any_match_t &operator()() { return *this; }

    template<typename UnaryPred>
    any_pred_t<UnaryPred> operator()(UnaryPred pred) { return any_pred_t<UnaryPred>(pred); }

    template<typename T>
    std::vector<T> &&stream(streamer_t<T> &, std::vector<T> &values) {
        return !values.empty(); 
    }
} any_match;


template<typename UnaryPred>
class all_match {
public:
    all_match(UnaryPred p) : pred(p) {}

    template<typename T>
    bool stream(streamer_t<T> &st, std::vector<T> &values) {
        return none([this](auto x) { return !pred(x); }).stream(st, values);
    }

private:
    UnaryPred pred;
};


}  // namespace streamer

#endif
