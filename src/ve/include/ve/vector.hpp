#pragma once

#include "ve/internal/pod_wrapper.hpp"

#include <cmath>
#include <cstddef>
#include <functional>
#include <ostream>
#include <type_traits>
#include <utility>

namespace ve {

template <template <class> class M, class T, size_t N = sizeof(M<T>) / sizeof(T)>
class Vector : public internal::PodWrapper<M, T, N> {
    static_assert(std::is_trivial<M<T>>());
    static_assert(std::is_standard_layout<M<T>>());
    static_assert(sizeof(M<T>) == N * sizeof(T));

public:
    using internal::PodWrapper<M, T, N>::PodWrapper;

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Vector(const Vector<M, U, N>& other)
    {
        *this = other;
    }

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Vector& operator=(const Vector<M, U, N>& other)
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

    constexpr Vector operator-() const
    {
        Vector result = *this;
        for (size_t i = 0; i < N; i++) {
            result[i] = -result[i];
        }
        return result;
    }

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Vector& operator+=(const Vector<M, U, N>& other)
    {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] += other[i];
        }
        return *this;
    }

    template <class U> requires std::is_convertible_v<U, T>
    constexpr Vector& operator-=(const Vector<M, U, N>& other)
    {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] -= other[i];
        }
        return *this;
    }

    template <class S> requires std::is_convertible_v<S, T>
    constexpr Vector& operator*=(const S& scalar)
    {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] *= scalar;
        }
        return *this;
    }

    template <class S> requires std::is_convertible_v<S, T>
    constexpr Vector& operator/=(const S& scalar)
    {
        for (size_t i = 0; i < N; i++) {
            (*this)[i] /= scalar;
        }
        return *this;
    }
};

template <template <class> class M, class U, class V, size_t N>
constexpr auto operator+(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    using R = decltype(std::declval<U>() + std::declval<V>());
    Vector<M, R, N> result = lhs;
    result += rhs;
    return result;
}

template <template <class> class M, class U, class V, size_t N>
constexpr auto operator-(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    using R = decltype(std::declval<U>() - std::declval<V>());
    Vector<M, R, N> result = lhs;
    result -= rhs;
    return result;
}

template <template <class> class M, class T, class S, size_t N>
constexpr auto operator*(const Vector<M, T, N>& vector, const S& scalar)
{
    using R = decltype(std::declval<T>() * std::declval<S>());
    Vector<M, R, N> result = vector;
    result *= scalar;
    return result;
}

template <template <class> class M, class T, class S, size_t N>
constexpr auto operator*(const S& scalar, const Vector<M, T, N>& vector)
{
    return vector * scalar;
}

template <template <class> class M, class T, class S, size_t N>
constexpr auto operator/(const Vector<M, T, N>& vector, const S& scalar)
{
    using R = decltype(std::declval<T>() / std::declval<S>());
    Vector<M, R, N> result = vector;
    result /= scalar;
    return result;
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator==(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    for (size_t i = 0; i < N; i++) {
        if (!(lhs[i] == rhs[i])) {
            return false;
        }
    }
    return true;
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator!=(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    return !(lhs == rhs);
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator<=(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    for (size_t i = 0; i < N; i++) {
        if (!(lhs[i] <= rhs[i])) {
            return false;
        }
    }
    return true;
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator>=(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    return rhs <= lhs;
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator<(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    return lhs <= rhs && lhs != rhs;
}

template <template <class> class M, class U, class V, size_t N>
constexpr bool operator>(const Vector<M, U, N>& lhs, const Vector<M, V, N>& rhs)
{
    return rhs < lhs;
}

template <template <class> class M, class T, size_t N>
constexpr auto length(const Vector<M, T, N>& vector) requires std::is_arithmetic_v<T>
{
    auto sqsum = T{0};
    for (size_t i = 0; i < N; i++) {
        sqsum += vector[i] * vector[i];
    }
    return std::sqrt(sqsum);
}

template <template <class> class M, class T, size_t N>
constexpr Vector<M, T, N> unit(const Vector<M, T, N>& vector)
    requires std::is_arithmetic_v<T>
{
    if (length(vector) > 0) {
        return vector / length(vector);
    } else {
        return vector;
    }
}

template <template <class> class M, class T, size_t N>
constexpr std::ostream& operator<<(std::ostream& output, const Vector<M, T, N>& vector)
{
    static_assert(N > 0);
    output << "[" << vector[0];
    for (size_t i = 1; i < N; i++) {
        output << ", " << vector[i];
    }
    output << "]";
    return output;
}

} // namespace ve

namespace std {

template <template <class> class M, class T, size_t N>
struct less<ve::Vector<M, T, N>> {
    constexpr bool operator()(
        const ve::Vector<M, T, N>& lhs, const ve::Vector<M, T, N>& rhs) const
    {
        size_t i = 0;
        while (lhs[i] == rhs[i] && i < N) {
            i++;
        }
        return i < N && lhs[i] < rhs[i];
    }
};

template <template <class> class M, class T, size_t N>
struct greater<ve::Vector<M, T, N>> {
    constexpr bool operator()(
        const ve::Vector<M, T, N>& lhs, const ve::Vector<M, T, N>& rhs) const
    {
        return less<ve::Vector<M, T, N>>{}(rhs, lhs);
    }
};

template <template <class> class M, class T, size_t N>
struct less_equal<ve::Vector<M, T, N>> {
    constexpr bool operator()(
        const ve::Vector<M, T, N>& lhs, const ve::Vector<M, T, N>& rhs) const
    {
        return less<ve::Vector<M, T, N>>{}(lhs, rhs) || lhs == rhs;
    }
};

template <template <class> class M, class T, size_t N>
struct greater_equal<ve::Vector<M, T, N>> {
    constexpr bool operator()(
        const ve::Vector<M, T, N>& lhs, const ve::Vector<M, T, N>& rhs) const
    {
        return greater<ve::Vector<M, T, N>>{}(lhs, rhs) || lhs == rhs;
    }
};

} // namespace std

