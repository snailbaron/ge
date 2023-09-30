#pragma once

#include <cmath>
#include <ostream>

namespace gx {

template <class T, class Tag = void>
struct Vector {
    constexpr Vector& operator+=(const Vector& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector& operator-=(const Vector& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vector& operator*=(const T& scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vector& operator/=(const T& scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    [[nodiscard]] constexpr T length() const
    {
        return std::sqrt(x * x + y * y);
    }

    constexpr void normalize()
    {
        if (auto l = length(); l != 0) {
            (*this) /= l;
        } else {
            (*this) = {0, 0};
        }
    }

    [[nodiscard]] constexpr Vector normalized() const
    {
        auto copy = *this;
        copy.normalize();
        return copy;
    }

    constexpr void resize(const T& newSize)
    {
        if (auto l = length(); l != 0) {
            (*this) *= newSize / l;
        } else {
            (*this) = {0, 0};
        }
    }

    [[nodiscard]] constexpr Vector resized(const T& newSize) const
    {
        auto copy = *this;
        copy.resize(newSize);
        return copy;
    }

    T x {};
    T y {};
};

template <class T, class Tag>
std::ostream& operator<<(std::ostream& output, const Vector<T, Tag>& vector)
{
    return output << "(" << vector.x << ", " << vector.y << ")";
}

template <class T, class Tag>
constexpr Vector<T, Tag> operator+(
    Vector<T, Tag> lhs, const Vector<T, Tag>& rhs)
{
    lhs += rhs;
    return lhs;
}

template <class T, class Tag>
constexpr Vector<T, Tag> operator-(
    Vector<T, Tag> lhs, const Vector<T, Tag>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <class T, class Tag>
constexpr Vector<T, Tag> operator*(Vector<T, Tag> vector, const T& scalar)
{
    vector *= scalar;
    return vector;
}

template <class T, class Tag>
constexpr Vector<T, Tag> operator*(const T& scalar, Vector<T, Tag> vector)
{
    vector *= scalar;
    return vector;
}

template <class T, class Tag>
constexpr Vector<T, Tag> operator/(Vector<T, Tag> vector, const T& scalar)
{
    vector /= scalar;
    return vector;
}

template <class T, class Tag>
constexpr Vector<T, Tag> operator-(const Vector<T, Tag>& vector)
{
    return {-vector.x, -vector.y};
}

template <class U, class V, class Tag>
constexpr Vector<U, Tag> cast(const Vector<V, Tag>& source)
{
    return {.x = static_cast<U>(source.x), .y = static_cast<U>(source.y)};
}

template <class T, class Tag = void>
struct Point {
    constexpr Point& operator+=(const Vector<T, Tag>& vector)
    {
        x += vector.x;
        y += vector.y;
        return *this;
    }

    constexpr Point& operator-=(const Vector<T, Tag>& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }

    T x {};
    T y {};
};

template <class T, class Tag>
std::ostream& operator<<(std::ostream& output, const Point<T, Tag>& point)
{
    return output << "(" << point.x << ", " << point.y << ")";
}

template <class T, class Tag>
constexpr Point<T, Tag> operator+(
    Point<T, Tag> point, const Vector<T, Tag>& vector)
{
    point += vector;
    return point;
}

template <class T, class Tag>
constexpr Point<T, Tag> operator-(
    Point<T, Tag> point, const Vector<T, Tag>& vector)
{
    point -= vector;
    return point;
}

template <class T, class Tag>
constexpr Vector<T, Tag> operator-(
    const Point<T, Tag>& lhs, const Point<T, Tag>& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

template <class T, class Tag>
constexpr T distance(const Point<T, Tag>& lhs, const Point<T, Tag>& rhs)
{
    return (lhs - rhs).length();
}

template <class U, class V, class Tag>
constexpr Point<U, Tag> cast(const Point<V, Tag>& source)
{
    return {.x = static_cast<U>(source.x), .y = static_cast<U>(source.y)};
}

template <class T, class Tag>
struct Rectangle {
    static constexpr Rectangle atPosition(
        const Point<T, Tag>& position, const Vector<T, Tag>& size)
    {
        return {
            .x = position.x - size.x / 2,
            .y = position.y - size.y / 2,
            .w = size.x,
            .h = size.y,
        };
    }

    constexpr bool contains(const Point<T, Tag>& point) const
    {
        return
            point.x >= x && point.x <= x + w &&
            point.y >= y && point.y <= y + h;
    }

    constexpr Vector<T, Tag> size() const
    {
        return {w, h};
    }

    constexpr Point<T, Tag> middlePoint() const
    {
        return {x + w / 2, y + h / 2};
    }

    T x {};
    T y {};
    T w {};
    T h {};
};

template <class T, class Tag>
std::ostream& operator<<(
    std::ostream& output, const Rectangle<T, Tag>& rectangle)
{
    return output << "(" << rectangle.x << ", " << rectangle.y <<
        ", " << rectangle.w << ", " << rectangle.h << ")";
}

template <class U, class V, class Tag>
constexpr Rectangle<U, Tag> cast(const Rectangle<V, Tag>& source)
{
    return {
        .x = static_cast<U>(source.x),
        .y = static_cast<U>(source.y),
        .w = static_cast<U>(source.w),
        .h = static_cast<U>(source.h),
    };
}

} // namespace gx
