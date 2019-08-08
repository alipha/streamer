#ifndef STREAMER_COMBINE_HPP
#define STREAMER_COMBINE_HPP

#include "base.hpp"
#include <algorithm>


namespace streamer {


template<typename Cont>
class concat : public detail::stream_manip<concat<Cont> > {
public:
    concat(Cont &&c) : cont(std::move(c)) {}

    template<typename T>
    streamer_t<T> &&stream(streamer_t<T> &st, std::vector<T> &values) {
        auto r = detail::range(cont);
        values.insert(values.end(), std::make_move_iterator(r.first), std::make_move_iterator(r.second));
        return std::move(st);
    }

private:
    Cont cont;
};


template<typename Cont, typename BiFunc>
class zip : public detail::stream_manip<zip<Cont, BiFunc> > {
public:
    zip(Cont &&c, BiFunc f) : cont(std::forward<Cont>(c)), func(std::move(f)) {}

    template<typename T>
    auto stream(streamer_t<T> &st, std::vector<T> &values) {
        auto r = detail::range(cont);
        auto it2 = r.first;
        using U = typename detail::remove_ref_cv<decltype(func(std::move(values[0]), std::move(*it2)))>::type;

        std::vector<U> out;

        for(auto it = values.begin(); it != values.end() && it2 != r.second; ++it, ++it2) {
            out.push_back(func(std::move(*it), std::move(*it2)));
        }

        return streamer_t<U>(std::move(out));
    }
    
private:
    Cont cont;
    BiFunc func;
};


} // namespace streamer

#endif
