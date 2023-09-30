#include <arg.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    auto parser = arg::Parser{};
    parser.config.packPrefix = "/";
    parser.helpKeys("/h", "/help");
    auto a = parser.flag().keys("/a");
    auto b = parser.flag().keys("/b");
    auto n = parser.option<int>().keys("/n", "/number");
    parser.parse(argc, argv);

    std::cout <<
        "a: " << a << "\n" <<
        "b: " << b << "\n" <<
        "n: " << n << "\n";
}
