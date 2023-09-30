#include "e.hpp"

#include "build-info.hpp"

#include <filesystem>

namespace e {

Error::Error(std::source_location sourceLocation)
    : _sourceLocation(sourceLocation)
{ }

const char* Error::what() const noexcept
{
    if (_cache.empty()) {
        auto relativeFilePath = std::filesystem::proximate(
            _sourceLocation.file_name(), internal::SOURCE_ROOT).string();

        auto stream = std::ostringstream{};
        stream <<
            relativeFilePath << ":" <<
            _sourceLocation.line() << ":" <<
            _sourceLocation.column() << " (" <<
            _sourceLocation.function_name() << ") " <<
            _message;
        _cache = stream.str();
    }
    return _cache.c_str();
}

inline void check(bool condition, std::source_location sourceLocation)
{
    if (!condition) {
        throw Error{sourceLocation} << "check failed";
    }
}

} // namespace e
