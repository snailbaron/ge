#include <fi/memory_mapped_file.hpp>

#include <stdexcept>

#ifdef __linux
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace fi {

MemoryMappedFile::MemoryMappedFile(const fs::path& path)
{
    if (!fs::exists(path)) {
        throw std::runtime_error{"MemoryMappedFile: path does not exist: " + path.string()};
    }

#ifdef __linux__
    _fd = open(path.string().c_str(), O_RDONLY); // NOLINT
    if (_fd == -1) {
        throw std::runtime_error{
            "MemoryMappedFile: failed to open file: " + path.string()};
    }

    size_t fileSize = 0;
    {
        struct stat sb{};
        if (fstat(_fd, &sb) == -1) {
            throw std::runtime_error{
                "MemoryMappedFile: failed to fstat file: " + path.string()};
        }
        fileSize = sb.st_size;
    }

    void* address = mmap(nullptr, fileSize, PROT_READ, MAP_PRIVATE, _fd, 0);
    if (address == MAP_FAILED) {
        throw std::runtime_error{
            "MemoryMappedFile: failed to mmap file: " + path.string()};
    }

    _span = {static_cast<std::byte*>(address), static_cast<size_t>(fileSize)};
#elif defined(_WIN32)
    _fileHandle = CreateFile(
        path.string().c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (_fileHandle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error{"CreateFile failed: " + GetLastError()};
    }

    LARGE_INTEGER fileSize{};
    if (!GetFileSizeEx(_fileHandle, &fileSize)) {
        throw std::runtime_error{"GetFileSizeEx failed: " + GetLastError()};
    }

    _fileMappingHandle =
        CreateFileMapping(_fileHandle, NULL, PAGE_READONLY, 0, 0, NULL);
    if (_fileMappingHandle == NULL) {
        throw std::runtime_error{"CreateFileMapping failed: " + GetLastError()};
    }

    LPVOID address = MapViewOfFile(_fileMappingHandle, FILE_MAP_READ, 0, 0, 0);
    if (!address) {
        throw std::runtime_error{"MapViewOfFile failed: " + GetLastError()};
    }

    _span = {
        static_cast<std::byte*>(address),
        static_cast<size_t>(fileSize.QuadPart)
    };
#endif
}

MemoryMappedFile::MemoryMappedFile(MemoryMappedFile&& other) noexcept
#ifdef linux
    : _fd(other._fd)
#elif defined(_WIN32)
    : _fileHandle(other._fileHandle)
    , _fileMappingHandle(other._fileMappingHandle)
#endif
{
    std::swap(_span, other._span);
}

MemoryMappedFile::~MemoryMappedFile()
{
    clear();
}

MemoryMappedFile& MemoryMappedFile::operator=(MemoryMappedFile&& other) noexcept
{
    clear();

#ifdef linux
    std::swap(_fd, other._fd);
#elif defined(_WIN32)
    std::swap(_fileHandle, other._fileHandle);
    std::swap(_fileMappingHandle, other._fileMappingHandle);
#endif

    std::swap(_span, other._span);

    return *this;
}

std::span<const std::byte> MemoryMappedFile::span() const
{
    return _span;
}

void MemoryMappedFile::clear() noexcept
{
    if (!_span.empty()) {
#ifdef linux
        munmap(_span.data(), _span.size());
        close(_fd);
        _fd = -1;
#elif defined(_WIN32)
        UnmapViewOfFile(_span.data());
        CloseHandle(_fileHandle);
        _fileHandle = INVALID_HANDLE_VALUE;
        _fileMappingHandle = INVALID_HANDLE_VALUE;
#endif
        _span = {};
    }
}

} // namespace fi
