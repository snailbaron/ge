#include <arg.hpp>

#include <iostream>
#include <istream>

struct Point {
    int x;
    int y;
};

std::istream& operator>>(std::istream& input, Point& point)
{
    return input >> point.x >> point.y;
}

int main(int argc, char* argv[])
{
    auto parser = arg::Parser{};
    parser.helpKeys("-h", "--help");
    auto s = parser.option<short>()
        .keys("--short");
    auto ull = parser.option<unsigned long long>()
        .keys("--unsigned-long-long");
    auto b = parser.option<bool>()
        .keys("--bool");
    auto str = parser.option<std::string>()
        .keys("--string");
    auto p = parser.option<Point>()
        .keys("--point");
    parser.parse(argc, argv);

    std::cout <<
        "short: " << s << "\n" <<
        "unsigned long long: " << ull << "\n" <<
        "bool: " << b << "\n" <<
        "string: " << str << "\n" <<
        "point: " << p->x << ", " << p->y << "\n";
}
