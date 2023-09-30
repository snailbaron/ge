#pragma once

#include <cstddef>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace ve::internal {

template <template <class> class Pod, class T, size_t N>
class PodWrapper : public Pod<T> {
public:
    constexpr PodWrapper(std::initializer_list<T> l)
        : Pod<T>{createPod(l, std::make_index_sequence<N>{})}
    { }

    template <class... Ts>
    constexpr PodWrapper(Ts&&... args)
    requires std::conjunction_v<std::is_convertible<Ts, T>...>
        : Pod<T>{static_cast<T&&>(args)...}
    { }

private:
    template <size_t... Is>
    constexpr Pod<T> createPod(
        std::initializer_list<T> l, std::index_sequence<Is...>)
    {
        return Pod<T>{*(l.begin() + Is)...};
    }
};

} // namespace ve::internal
