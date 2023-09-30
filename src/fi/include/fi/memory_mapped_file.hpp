#pragma once

#include <cstddef>
#include <filesystem>
#include <span>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fi {

class MemoryMappedFile {
public:
    MemoryMappedFile() = default;
    explicit MemoryMappedFile(const std::filesystem::path& path);
    MemoryMappedFile(const MemoryMappedFile& other) = delete;
    MemoryMappedFile(MemoryMappedFile&& other) noexcept;
    ~MemoryMappedFile();

    MemoryMappedFile& operator=(const MemoryMappedFile& other) = delete;
    MemoryMappedFile& operator=(MemoryMappedFile&& other) noexcept;

    [[nodiscard]] std::span<const std::byte> span() const;

    void clear() noexcept;

private:
#ifdef __linux__
    int _fd = -1;
#elif defined(_WIN32)
    HANDLE _fileHandle = INVALID_HANDLE_VALUE;
    HANDLE _fileMappingHandle = INVALID_HANDLE_VALUE;
#endif
    std::span<std::byte> _span;
};

} // namespace fi
