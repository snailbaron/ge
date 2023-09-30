#include <catch2/catch_test_macros.hpp>

#include <arg.hpp>

#include <string>

TEST_CASE("Basic arg test")
{
    auto f = arg::flag()
        .keys("-v")
        .help("some flag");
    auto x = arg::option<int>()
        .keys("-x", "--value")
        .markRequired()
        .help("some value");
    auto y = arg::option<int>()
        .keys("-y", "--another-value")
        .defaultValue(10)
        .metavar("SOMETHING")
        .help("another value");
    auto z = arg::argument<std::string>()
        .metavar("PATH");
}
