#pragma once

#include <type_traits>

template <typename T>
concept ContainerConcept = requires(T t) {
    { t.data() };
    { t.size() } -> std::convertible_to<std::size_t>;
    typename T::value_type;
};
