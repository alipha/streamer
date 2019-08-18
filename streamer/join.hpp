#ifndef STREAMER_JOIN_HPP
#define STREAMER_JOIN_HPP

#include "base.hpp"
#include <sstream>
#include <string>


namespace streamer {


template<typename Char>
class join : public detail::step_wrapper<join<Char> > {
public:
    join(const Char *delimiter) : delim(delimiter) {}
    join(std::basic_string<Char> delimiter) : delim(std::move(delimiter)) {}

    template<typename T>
    std::basic_string<Char> stream(streamer_t<T> &, std::unique_ptr<detail::step<T> > &s, bool &unbounded) {
        if(unbounded)
            throw unbounded_stream("cannot use join on an unbounded stream");

        std::optional<T> value = s->get();
        if(!value)
            return {};
        std::basic_stringstream<Char> out;

        out << *std::move(value);

        while((value = s->get()))
            out << delim << *std::move(value);

        return out.str();
    }

private:
    std::basic_string<Char> delim;
};


} // namespace streamer


#endif
