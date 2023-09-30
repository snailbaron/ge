#pragma once

#include <limits>
#include <ostream>
#include <stdexcept>

namespace gx {

struct UiCoordinate {
    constexpr UiCoordinate& operator+=(const UiCoordinate& other)
    {
        pixels += other.pixels;
        fraction += other.fraction;
        return *this;
    }

    constexpr UiCoordinate& operator-=(const UiCoordinate& other)
    {
        pixels -= other.pixels;
        fraction -= other.fraction;
        return *this;
    }

    constexpr UiCoordinate& operator*=(float scalar)
    {
        pixels *= scalar;
        fraction *= scalar;
        return *this;
    }

    constexpr UiCoordinate& operator/=(float scalar)
    {
        pixels /= scalar;
        fraction /= scalar;
        return *this;
    }

    float pixels = 0.f;
    float fraction = 0.f;
};

inline namespace literals {

constexpr UiCoordinate operator ""_px(long double pixels)
{
    return {.pixels = (float)pixels};
}

constexpr UiCoordinate operator ""_px(unsigned long long pixels)
{
    return {.pixels = (float)pixels};
}

constexpr UiCoordinate operator ""_fr(long double fraction)
{
    return {.fraction = (float)fraction};
}

constexpr UiCoordinate operator ""_fr(unsigned long long fraction)
{
    return {.fraction = (float)fraction};
}

} // namespace literals

inline std::ostream& operator<<(
    std::ostream& output, const UiCoordinate& windowPosition)
{
    return output << windowPosition.fraction << std::showpos <<
        windowPosition.pixels << "px" << std::noshowpos;
}

constexpr UiCoordinate operator+(UiCoordinate lhs, const UiCoordinate& rhs)
{
    lhs += rhs;
    return lhs;
}

constexpr UiCoordinate operator-(UiCoordinate lhs, const UiCoordinate& rhs)
{
    lhs -= rhs;
    return lhs;
}

constexpr UiCoordinate operator*(UiCoordinate length, float scalar)
{
    length *= scalar;
    return length;
}

constexpr UiCoordinate operator*(float scalar, UiCoordinate length)
{
    length *= scalar;
    return length;
}

constexpr UiCoordinate operator/(UiCoordinate length, float scalar)
{
    length /= scalar;
    return length;
}

} // namespace gx
