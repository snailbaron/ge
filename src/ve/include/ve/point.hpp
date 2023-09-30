#pragma once

#include "ve/internal/pod_wrapper.hpp"
#include "ve/vector.hpp"

#include <cstddef>
#include <ostream>
#include <type_traits>
#include <utility>

namespace ve {

template <template <class> class M, class T, size_t N = sizeof(M<T>) / sizeof(T)>
class Point : public internal::PodWrapper<M, T, N> {
    static_assert(std::is_trivial<M<T>>());
    static_assert(std::is_standard_layout<M<T>>());
    static_assert(sizeof(M<T>) == N * sizeof(T));

public:
    using internal::PodWrapper<M, T, N>::PodWrapper;

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Point(const Point<M, U, N>& other)
    {
        *this = other;
    }

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Point& operator=(const Point<M, U, N>& other)
    {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] = other[i];
        }
        return *this;
    }

    constexpr T& operator[](size_t i)
    {
        return *(reinterpret_cast<T*>(this) + i);
    }

    constexpr const T& operator[](size_t i) const
    {
        return *(reinterpret_cast<const T*>(this) + i);
    }

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Point& operator+=(const Vector<M, U, N>& vector)
    {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] += vector[i];
        }
        return *this;
    }

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Point& operator-=(const Vector<M, U, N>& vector)
    {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] -= vector[i];
        }
        return *this;
    }
};

template <template <class> class M, class U, class V, size_t N>
constexpr auto operator+(const Point<M, U, N>& point, const Vector<M, V, N>& vector)
{
    using R = decltype(std::declval<U>() + std::declval<V>());
    Point<M, R, N> result = point;
    result += vector;
    return result;
}

template <template <class> class M, class U, class V, size_t N>
constexpr auto operator-(const Point<M, U, N>& point, const Vector<M, V, N>& vector)
{
    using R = decltype(std::declval<U>() - std::declval<V>());
    Point<M, R, N> result = point;
    result -= vector;
    return result;
}

template <template <class> class M, class U, class V, size_t N>
constexpr auto operator-(const Point<M, U, N>& lhs, const Point<M, V, N>& rhs)
{
    using R = decltype(std::declval<U>() - std::declval<V>());
    Vector<M, R, N> result;
    for (size_t i = 0; i < N; i++) {
        result[i] = lhs[i] - rhs[i];
    }
    return result;
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator==(const Point<M, U, N>& lhs, const Point<M, V, N>& rhs)
{
    for (size_t i = 0; i < N; i++) {
        if (!(lhs[i] == rhs[i])) {
            return false;
        }
    }
    return true;
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator!=(const Point<M, U, N>& lhs, const Point<M, V, N>& rhs)
{
    return !(lhs == rhs);
}

template <template <class> class M, class U, class V, size_t N>
constexpr auto distance(const Point<M, U, N>& lhs, const Point<M, V, N>& rhs)
{
    return length(lhs - rhs);
}

template <template <class> class M, class T, size_t N>
constexpr std::ostream& operator<<(std::ostream& output, const Point<M, T, N>& point)
{
    static_assert(N > 0);
    output << "(" << point[0];
    for (size_t i = 1; i < N; i++) {
        output << ", " << point[i];
    }
    output << ")";
    return output;
}

} // namespace ve
