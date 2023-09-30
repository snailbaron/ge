#include <gx/error.hpp>

#include "build-info.hpp"

#include <SDL.h>

#include <filesystem>
#include <sstream>

namespace gx {

Error::Error(
    std::string_view message, std::source_location sourceLocation)
{
    auto relativeFilePath = std::filesystem::proximate(
        sourceLocation.file_name(), SOURCE_ROOT).string();

    auto stream = std::ostringstream{};
    stream <<
        relativeFilePath << ":" <<
        sourceLocation.line() << ":" <<
        sourceLocation.column() << " (" <<
        sourceLocation.function_name() << ") " <<
        message;

    _fullMessage = stream.str();
}

const char* Error::what() const noexcept
{
    return _fullMessage.c_str();
}

SdlError::SdlError(std::source_location sourceLocation)
    : Error(SDL_GetError(), sourceLocation)
{ }

void sdlCheck(int returnCode, std::source_location sourceLocation)
{
    if (returnCode < 0) {
        throw SdlError{sourceLocation};
    }
}

} // namespace gx
