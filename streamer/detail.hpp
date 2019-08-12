#ifndef STREAMER_DETAIL_HPP
#define STREAMER_DETAIL_HPP

#include <iterator>
#include <functional>
#include <optional>
#include <type_traits>


namespace streamer {
namespace detail {


template<typename Derived>
class step_wrapper {
public:
    constexpr step_wrapper() noexcept = default;

    constexpr Derived &get_derived() noexcept { return static_cast<Derived&>(*this); }

protected:
    step_wrapper(const step_wrapper<Derived> &) noexcept = default;
    step_wrapper(step_wrapper<Derived> &&) noexcept = default;
    step_wrapper<Derived> &operator=(const step_wrapper<Derived> &) noexcept = default;
    step_wrapper<Derived> &operator=(step_wrapper<Derived> &&) noexcept = default;
};


template<typename T>
class step {
public:
    constexpr step() noexcept = default;

    step(const step<T> &) = delete;
    step(step<T> &&) = delete;
    step<T> &operator=(const step<T> &) = delete;
    step<T> &operator=(step<T> &&) = delete;

    virtual std::optional<T> get() = 0;
};



template<typename T>
struct remove_ref_cv {
    typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
};



template<typename Cont, typename T>
class cont_source : public step<T> {
public:
    cont_source(const Cont &c) : cont(c), it(std::begin(cont)) {}
    cont_source(Cont &&c) : cont(std::move(c)), it(std::begin(cont)) {}

    std::optional<T> get() override {
        if(it == cont.end())
            return {};
        T value = std::move(*it);
        ++it;
        return {std::move(value)};
    }
private:
    Cont cont;
    decltype(std::begin(cont)) it;
};


template<typename It, typename T>
class it_source : public step<T> {
public:
    it_source(It &&begin_it, It &&end_t) : it(std::move(begin_it)), end(std::move(end_it)) {}

    std::optional<T> get() override {
        if(it == end)
            return {};
        T value = std::move(*it);
        ++it;
        return {std::move(value)};
    }
private:
    It it;
    It end;
};



template<typename UnaryFunc>
auto member_mapper(UnaryFunc &&f) { return f; }

template<typename T, typename U>
constexpr auto member_mapper(U T::*p) noexcept {
    return [p](const T &obj) { return obj.*p; };
}

template<typename T, typename U>
constexpr auto member_mapper(U (T::*p)() const) noexcept {
    return [p](const T &obj) { return (obj.*p)(); };
}

template<typename T, typename U>
constexpr auto member_mapper(U (T::*p)() const noexcept) noexcept {
    return [p](const T &obj) { return (obj.*p)(); };
}


template<typename T, typename U, typename Comp>
auto member_comparer_custom(U T::*p, Comp &&comp) {
    return [p, comp](const T &left, const T &right) { return comp(left.*p, right.*p); };
}

template<typename T, typename U, typename Comp>
auto member_comparer_custom(U (T::*p)() const, Comp &&comp) {
    return [p, comp](const T &left, const T &right) { return comp((left.*p)(), (right.*p)()); };
}

template<typename T, typename U, typename Comp>
auto member_comparer_custom(U (T::*p)() const noexcept, Comp &&comp) {
    return [p, comp](const T &left, const T &right) { return comp((left.*p)(), (right.*p)()); };
}

template<typename Comp>
auto member_comparer(Comp &&comp) { return comp; }

template<typename T, typename U>
auto member_comparer(U T::*p) { return member_comparer_custom(p, std::less<U>()); }

template<typename T, typename U>
auto member_comparer(U (T::*p)() const) { return member_comparer_custom(p, std::less<U>()); }

template<typename T, typename U>
auto member_comparer(U (T::*p)() const noexcept) { return member_comparer_custom(p, std::less<U>()); }

  
} // namespace detail
} // namespace streamer

#endif
