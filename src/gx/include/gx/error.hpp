#pragma once

#include <exception>
#include <source_location>
#include <string>
#include <string_view>

namespace gx {

class Error : public std::exception {
public:
    explicit Error(
        std::string_view message,
        std::source_location sourceLocation = std::source_location::current());

    const char* what() const noexcept override;

private:
    std::string _fullMessage;
};

class SdlError : public Error {
public:
    SdlError(
        std::source_location sourceLocation = std::source_location::current());
};

template <class T>
T* sdlCheck(
    T* ptr,
    std::source_location sourceLocation = std::source_location::current())
{
    if (!ptr) {
        throw SdlError{sourceLocation};
    }
    return ptr;
}

void sdlCheck(
    int returnCode,
    std::source_location sourceLocation = std::source_location::current());

} // namespace gx
