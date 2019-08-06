#ifndef STREAMER_OPTIONAL_HPP
#define STREAMER_OPTIONAL_HPP

#include "base.hpp"
#include <optional>


namespace streamer {


template<typename T>
struct or_default {
    or_default(T value) : val(value) {}
    T val;
};


template<typename T, typename U>
T operator>>(std::optional<T> &&opt, or_default<U> &&def) {
    return std::forward<std::optional<T> >(opt).value_or(std::move(def.val));
}


} // namespace streamer

#endif
