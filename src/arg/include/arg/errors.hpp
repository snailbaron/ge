#pragma once

#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <variant>

namespace arg::err {

struct InvalidValueGiven {
    std::string keys;
    std::string value;
};

struct RequiredOptionNotSet {
    std::string keys;
};

struct RequiredOptionValueNotGiven {
    std::string key;
};

struct UnexpectedArgument {
    std::string argument;
};

struct UnexpectedOptionValueGiven {
    std::string key;
    std::string value;
};

using Error = std::variant<
    InvalidValueGiven,
    RequiredOptionNotSet,
    RequiredOptionValueNotGiven,
    UnexpectedArgument,
    UnexpectedOptionValueGiven
>;

inline void print(std::ostream& output, const Error& error)
{
    std::visit([&output] (auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same<T, InvalidValueGiven>()) {
            output << "invalid value for option " << arg.keys <<
                ": " << arg.value << "\n";
        } else if constexpr (std::is_same<T, RequiredOptionNotSet>()) {
            output << "required option (" << arg.keys << ") is not set\n";
        } else if constexpr (std::is_same<T, RequiredOptionValueNotGiven>()) {
            output << "option " << arg.key <<
                " requires a value, but it was not provied\n";
        } else if constexpr (std::is_same<T, UnexpectedArgument>()) {
            output << "unexpected argument: " << arg.argument << "\n";
        } else if constexpr (std::is_same<T, UnexpectedOptionValueGiven>()) {
            output << "option " << arg.key <<
                " does not require a value, but " << arg.value <<
                " was provided\n";
        } else {
            throw std::logic_error{
                "cannot print an error of type " +
                    std::string{typeid(arg).name()}};
        }
    }, error);
}

} // namespace arg::err
