#pragma once

#include <type_traits>

template <typename, typename = std::void_t<>> struct has_function : std::false_type
{
};

template <typename T> struct has_data_fn<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type
{
};

template <typename T> struct has_size_fn<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type
{
};

template <typename T>
concept ContainerConcept = has_data_fn<T>::value && has_size_fn<T>::value;
