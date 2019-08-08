#ifndef STREAMER_STREAMER_HPP
#define STREAMER_STREAMER_HPP

#include "base.hpp"
#include <algorithm>
#include <iterator>


namespace streamer {

    
template<typename UnaryFunc>
class mapper : public detail::stream_manip<mapper<UnaryFunc> > {
public:
    mapper(UnaryFunc f) : func(std::move(f)) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::vector<T> &values) {
        auto f = detail::member_mapper(func);
        using U = typename detail::remove_ref_cv<decltype(f(values[0]))>::type;

        std::vector<U> out;
        std::transform(std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()), std::back_inserter(out), std::move(f));
        return streamer_t<U>(std::move(out));
    }
private:
    UnaryFunc func;
};


template<typename UnaryFunc>
class flat_mapper : public detail::stream_manip<flat_mapper<UnaryFunc> > {
public:
    flat_mapper(UnaryFunc f) : func(std::move(f)) {}

    template<typename T>
    auto stream(streamer_t<T> &, std::vector<T> &values) {
        //using Cont = typename detail::remove_ref_cv<decltype(func(std::move(values[0])))>::type;
        using U = typename detail::remove_ref_cv<decltype(*detail::range(func(std::move(values[0]))).first)>::type;
        
        std::vector<U> out;

        for(auto &value : values) {
            auto items = func(std::move(value));
            auto r = detail::range(items);
            out.insert(out.end(), std::make_move_iterator(r.first), std::make_move_iterator(r.second));
        }

        return streamer_t<U>(std::move(out));
    }

private:
    UnaryFunc func;
};


template<typename UnaryPred>
class count_if_t : public detail::stream_manip<count_if_t<UnaryPred> > {
public:
    count_if_t(UnaryPred p) : pred(std::move(p)) {}

    template<typename T>
    std::size_t stream(streamer_t<T> &, std::vector<T> &values) {
        return std::count_if(values.begin(), values.end(), std::move(pred));
    }

private:
    UnaryPred pred;
};


static class item_count_t : public detail::stream_manip<item_count_t> {
public:
    item_count_t &operator()() { return *this; }

    template<typename UnaryPred>
    count_if_t<UnaryPred> operator()(UnaryPred pred) { return count_if_t<UnaryPred>(std::move(pred)); }

    template<typename T>
    std::size_t &&stream(streamer_t<T> &, std::vector<T> &values) {
        return values.size(); 
    }
} item_count;


static class as_vector_t : public detail::stream_manip<as_vector_t> {
public:
    as_vector_t &operator()() { return *this; }

    template<typename T>
    std::vector<T> &&stream(streamer_t<T> &, std::vector<T> &values) {
        return std::move(values); 
    }
} as_vector;


template<typename UnaryFunc>
class each : public detail::stream_manip<each<UnaryFunc> > {
public:
    each(UnaryFunc f) : func(std::move(f)) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        std::for_each(values.begin(), values.end(), std::move(func));
        return std::move(st);
    }
private:
    UnaryFunc func;
};


template<typename It>
class to_iter : public detail::stream_manip<to_iter<It> > {
public:
    to_iter(It iter) : it(std::move(iter)) {}

    template<typename T>
    void stream(streamer_t<T> &st, std::vector<T> &values) {
        std::copy(values.begin(), values.end(), std::move(it));
    }

private:
    It it;
};


namespace detail {
    inline void streamer_unused_warnings() {
        item_count();
        as_vector();
    }
}

}  // namespace streamer

#endif
