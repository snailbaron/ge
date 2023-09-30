#include <ve.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cmath>
#include <set>
#include <sstream>
#include <type_traits>
#include <utility>

#define HAS_TYPE(EXPRESSION, TYPE) \
    static_assert(std::is_same<decltype(EXPRESSION), TYPE>());

#define CHECK_OP_EQ(OP, L, R, VALUE)                            \
    do {                                                        \
        SECTION(#OP) {                                          \
            static_assert(sizeof(#OP) == 3 && #OP[1] == '=');   \
            auto& ref = (L OP R);                               \
            auto sameType = std::is_same<                       \
                std::remove_reference_t<decltype(ref)>,         \
                decltype(VALUE)                                 \
            >();                                                \
            CHECK(sameType);                                    \
            CHECK(L == VALUE);                                  \
            CHECK(ref == VALUE);                                \
        }                                                       \
    } while (false)

#define CHECK_OP(OP, L, R, VALUE)                                           \
    do {                                                                    \
        SECTION(#OP) {                                                      \
            static_assert(sizeof(#OP) == 2);                                \
            auto v = L OP R;                                                \
            auto sameType = std::is_same<decltype(v), decltype(VALUE)>();   \
            CHECK(sameType);                                                \
            CHECK(v == VALUE);                                              \
        }                                                                   \
    } while (false)

template <class T> struct XYModel {
    T x;
    T y;
};
template <class T> using XYVector = ve::Vector<XYModel, T, 2>;
template <class T> using XYPoint = ve::Point<XYModel, T, 2>;

TEST_CASE("Initialization, explicit arguments")
{
    auto v = XYVector<int>{1, 2};
    auto p = XYPoint<int>{3, 4};
    CHECK(v.x == 1);
    CHECK(v.y == 2);
    CHECK(v[0] == 1);
    CHECK(v[1] == 2);
    CHECK(p.x == 3);
    CHECK(p.y == 4);
    CHECK(p[0] == 3);
    CHECK(p[1] == 4);
}

TEST_CASE("Initialization default constructor", "[initialization]")
{
    auto v = XYVector<int>{};
    CHECK(v.x == 0);
    CHECK(v.y == 0);

    auto p = XYPoint<int>{};
    CHECK(p.x == 0);
    CHECK(p.y == 0);
}

TEST_CASE("Default initialization", "[initialization]")
{
    XYVector<int> v;
    CHECK(v.x == 0);
    CHECK(v.y == 0);

    XYPoint<int> p;
    CHECK(p.x == 0);
    CHECK(p.y == 0);
}

TEST_CASE("Vector conversion")
{
    auto shortVector = XYVector<short>{1, 2};
    auto longLongVector = XYVector<long long>{3, 4};
    CHECK(longLongVector.x == 3);
    CHECK(longLongVector.y == 4);
    longLongVector = shortVector;
    CHECK(longLongVector.x == 1);
    CHECK(longLongVector.y == 2);
}

TEST_CASE("Point conversion")
{
    auto shortPoint = XYPoint<short>{1, 2};
    auto longLongPoint = XYPoint<long long>{3, 4};
    CHECK(longLongPoint.x == 3);
    CHECK(longLongPoint.y == 4);
    longLongPoint = shortPoint;
    CHECK(longLongPoint.x == 1);
    CHECK(longLongPoint.y == 2);
}

TEST_CASE("Vector and Point accessors")
{
    auto get_const = [] (const auto& object, size_t i) -> const auto& {
        return object[i];
    };
    auto get_non_const = [] (auto& object, size_t i) -> auto& {
        return object[i];
    };

    auto check = [&get_const, &get_non_const] (auto& object) {
        CHECK(get_const(object, 0) == 1);
        CHECK(get_const(object, 1) == 2);
        CHECK(get_non_const(object, 0) == 1);
        CHECK(get_non_const(object, 1) == 2);
        get_non_const(object, 0) = 3;
        get_non_const(object, 1) = 4;
        CHECK(get_const(object, 0) == 3);
        CHECK(get_const(object, 1) == 4);
        CHECK(get_non_const(object, 0) == 3);
        CHECK(get_non_const(object, 1) == 4);
    };

    auto v = XYVector<int>{1, 2};
    check(v);

    auto p = XYPoint<int>{1, 2};
    check(p);
}

TEST_CASE("Vector unary minus")
{
    auto v = XYVector<int>{1, 2};
    auto vv = -v;
    CHECK(vv.x == -1);
    CHECK(vv.y == -2);
}

TEST_CASE("Vector and Point arithmetics")
{
    auto sv = XYVector<short>{1, 2};
    auto lv = XYVector<long long>{3, 4};
    auto sp = XYPoint<short>{5, 6};
    auto lp = XYPoint<long long>{7, 8};
    auto fv = XYVector<float>{9.f, 10.f};
    long long l = 10;
    short s = 20;
    float f = 0.5f;

    CHECK_OP_EQ(+=, lv, sv, XYVector<long long>(4, 6));
    CHECK_OP(+, sv, lv, XYVector<long long>(4, 6));
    CHECK_OP(+, lv, sv, XYVector<long long>(4, 6));
    CHECK_OP_EQ(+=, lp, sv, XYPoint<long long>(8, 10));
    CHECK_OP(+, sp, lv, XYPoint<long long>(8, 10));
    CHECK_OP(+, lp, sv, XYPoint<long long>(8, 10));

    CHECK_OP_EQ(-=, lv, sv, XYVector<long long>(2, 2));
    CHECK_OP(-, sv, lv, XYVector<long long>(-2, -2));
    CHECK_OP(-, lv, sv, XYVector<long long>(2, 2));
    CHECK_OP_EQ(-=, lp, sv, XYPoint<long long>(6, 6));
    CHECK_OP(-, sp, lv, XYPoint<long long>(2, 2));
    CHECK_OP(-, lp, sv, XYPoint<long long>(6, 6));

    CHECK_OP_EQ(*=, lv, s, XYVector<long long>(60, 80));
    CHECK_OP(*, sv, l, XYVector<long long>(10, 20));
    CHECK_OP(*, l, sv, XYVector<long long>(10, 20));
    CHECK_OP(*, lv, s, XYVector<long long>(60, 80));
    CHECK_OP(*, s, lv, XYVector<long long>(60, 80));

    CHECK_OP_EQ(/=, fv, f, XYVector<float>(18, 20));
    CHECK_OP(/, sv, f, XYVector<float>(2, 4));

    CHECK_OP(-, sp, lp, XYVector<long long>(-2, -2));
    CHECK_OP(-, lp, sp, XYVector<long long>(2, 2));
}

TEST_CASE("Handy conversions")
{
    XYVector<short> v;

    HAS_TYPE(v, XYVector<short>);
    HAS_TYPE(1.f * v, XYVector<float>);
}

TEST_CASE("Vector Comparison")
{
    XYVector<short> s12 {1, 2};
    XYVector<long long> ll12 {1, 2};
    XYVector<int> i12 {1, 2};
    XYVector<int> i21 {2, 1};
    XYVector<int> i11 {1, 1};
    XYVector<int> i02 {0, 2};

    CHECK(s12 == ll12);

    CHECK(i12 != i21);
    CHECK_FALSE(i12 == i21);
    CHECK_FALSE(i12 < i21);
    CHECK_FALSE(i12 > i21);
    CHECK_FALSE(i12 <= i21);
    CHECK_FALSE(i12 >= i21);

    CHECK_FALSE(i12 == i11);
    CHECK(i12 != i11);
    CHECK(i12 > i11);
    CHECK(i12 >= i11);
    CHECK(i11 < i12);
    CHECK(i11 <= i12);

    CHECK_FALSE(i12 == i02);
    CHECK(i12 != i02);
    CHECK(i12 > i02);
    CHECK(i02 >= i02);
    CHECK(i02 < i12);
    CHECK(i02 <= i12);
}

TEST_CASE("Point Comparison")
{
    XYPoint<short> s12 {1, 2};
    XYPoint<long long> ll12 {1, 2};
    XYPoint<short> s11 {1, 1};
    XYPoint<short> s02 {0, 2};

    CHECK(s12 == ll12);

    CHECK_FALSE(s12 == s11);
    CHECK(s12 != s11);
    CHECK_FALSE(s12 == s02);
    CHECK(s12 != s02);
}

TEST_CASE("Can put into set")
{
    std::set<XYVector<int>> vs;
    std::set<XYPoint<int>> ps;
}

TEST_CASE("Utility Functions")
{
    SECTION("length") {
        auto v = XYVector<int>{1, 1};
        CHECK(length(v) == std::sqrt(2));
    }

    SECTION("unit") {
        auto v = XYVector<int>{1, 1};
        auto r = XYVector<decltype(std::sqrt(std::declval<int>()))>{
            1 / std::sqrt(2), 1 / std::sqrt(2)};
        CHECK(unit(v) == r);
    }

    SECTION("distance") {
        auto p1 = XYPoint<short>{1, 2};
        auto p2 = XYPoint<long long>{3, 5};
        CHECK(p1 - p2 == XYVector<long long>(-2, -3));
    }
}

TEST_CASE("Stream Output")
{
    SECTION("Vector") {
        auto v = XYVector<int>{1, 2};
        std::ostringstream stream;
        stream << v;
        CHECK(stream.str() == "[1, 2]");
    }

    SECTION("Point") {
        auto p = XYPoint<int>{1, 2};
        std::ostringstream stream;
        stream << p;
        CHECK(stream.str() == "(1, 2)");
    }
}
