#ifndef STREAMER_JOIN_HPP
#define STREAMER_JOIN_HPP

#include "base.hpp"
#include <sstream>
#include <string>


namespace streamer {

    
template<typename Char>
class join {
public:
    join(const Char *delimiter) : delim(delimiter) {}
    join(std::basic_string<Char> delimiter) : delim(std::move(delimiter)) {}

    template<typename T>
    std::basic_string<Char> stream(streamer_t<T> &, std::vector<T> &values) {
        if(values.empty())
            return {};
        std::basic_stringstream<Char> out;

        out << *values.begin();

        for(auto it = values.begin() + 1; it != values.end(); ++it)
            out << delim << std::move(*it);

        return out.str();
    }

private:
    std::basic_string<Char> delim;
};


} // namespace streamer

#endif
