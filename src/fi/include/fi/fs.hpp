#pragma once

#include <cstddef>
#include <filesystem>
#include <iterator>
#include <span>
#include <vector>

namespace fi {

std::filesystem::path userConfigPath();
std::filesystem::path globalConfigPath();

template <class T>
std::vector<T> read(const std::filesystem::path& path);

template <class T>
void write(const std::filesystem::path& path, const T* data, size_t size);

template <class T>
void write(const std::filesystem::path& path, const std::span<const T>& data);

extern template std::vector<std::byte> read<std::byte>(
    const std::filesystem::path& path);
extern template std::vector<unsigned char> read<unsigned char>(
    const std::filesystem::path& path);

extern template void write<std::byte>(
    const std::filesystem::path& path, const std::byte* data, size_t size);
extern template void write<unsigned char>(
    const std::filesystem::path& path, const unsigned char* data, size_t size);

extern template void write<std::byte>(
    const std::filesystem::path& path, const std::span<const std::byte>& data);
extern template void write<unsigned char>(
    const std::filesystem::path& path,
    const std::span<const unsigned char>& data);

} // namespace fi
