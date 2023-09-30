#include <arg.hpp>

#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    arg::helpKeys("-h", "--help");
    auto string = arg::option<std::string>()
        .keys("-s", "--string")
        .markRequired()
        .help("a string to print");
    auto number = arg::option<unsigned>()
        .keys("-n", "--number")
        .defaultValue(3)
        .help("number of times to print the string");
    arg::parse(argc, argv);

    for (unsigned i = 0; i < number; i++) {
        std::cout << string << "\n";
    }
}
