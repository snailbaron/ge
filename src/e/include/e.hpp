#pragma once

#include <exception>
#include <source_location>
#include <sstream>
#include <string>
#include <utility>

namespace e {

template <class T>
concept Streamable = requires(std::ostream output, T value)
{
    {output << value} -> std::same_as<std::ostream&>;
};

class Error : public std::exception {
public:
    explicit Error(
        std::source_location sourceLocation = std::source_location::current());

    const char* what() const noexcept override;

    template <Streamable T>
    Error& operator<<(T&& value) &
    {
        _cache.clear();
        _message = appendToString(std::move(_message), std::forward<T>(value));
        return *this;
    }

    template <Streamable T>
    Error operator<<(T&& value) &&
    {
        _cache.clear();
        _message = appendToString(std::move(_message), std::forward<T>(value));
        return *this;
    }

private:
    template <Streamable T>
    static std::string appendToString(std::string&& string, T&& value)
    {
        auto stream =
            std::ostringstream{std::move(string), std::ios_base::ate};
        stream << std::forward<T>(value);
        return stream.str();
    }

    std::string _message;
    std::source_location _sourceLocation;
    mutable std::string _cache;
};

void check(
    bool condition,
    std::source_location sourceLocation = std::source_location::current());

} // namespace e
