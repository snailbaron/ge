#include <ve.hpp>

#include <iostream>

template <class T> struct XYModel {
    T x;
    T y;
};
template <class T> using XYVector = ve::Vector<XYModel, T, 2>;
template <class T> using XYPoint = ve::Point<XYModel, T, 2>;

int main()
{
    auto v = XYVector<short>{1, 1};
    auto p1 = XYPoint<int>{3, 4};
    auto p2 = XYPoint<double>{4.0, 5.0};

    std::cout << p1 + v << "\n";  // (4, 5) - XYPoint<int>
    std::cout << p2 - v << "\n";  // (3, 4) - XYPoint<double>
    std::cout << p2 - p1 << "\n"; // [1, 1] - XYVector<double>
    std::cout << 2 * v << "\n";   // [2, 2] - XYVector<int>
    std::cout << 1ll * v << "\n"; // [1, 1] - XYVector<long long>

    static_assert(std::is_same<decltype(p1 + v), XYPoint<int>>());
    static_assert(std::is_same<decltype(p2 - v), XYPoint<double>>());
    static_assert(std::is_same<decltype(p2 - p1), XYVector<double>>());
    static_assert(std::is_same<decltype(2 * v), XYVector<int>>());
    static_assert(std::is_same<decltype(1ll * v), XYVector<long long>>());
}
