#pragma once

#include "Include.hpp"

namespace wsac::model
{

class Bytes
{
    void *_ptr;
    size_t _size;

  public:
    Bytes(void *ptr, const size_t size) : _ptr(ptr), _size(size)
    {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template <typename T> Bytes(T *p) // NOLINT(*-explicit-constructor)
    {
        _ptr = static_cast<void *>(p);
        _size = sizeof(T);
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template <ContainerConcept T> Bytes(T &c) // NOLINT(*-explicit-constructor)
    {
        _ptr = c.data();
        _size = c.size() * sizeof(typename T::value_type);
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template <ContainerConcept T> Bytes(const T &c) // NOLINT(*-explicit-constructor)
    {
        _ptr = const_cast<typename T::value_type *>(c.data());
        _size = c.size() * sizeof(typename T::value_type);
    }

    void *data() noexcept
    {
        return _ptr;
    }

    [[nodiscard]] const void *data() const noexcept
    {
        return _ptr;
    }

    [[nodiscard]] size_t size() const noexcept
    {
        return _size;
    }
};

class ConstBytes
{
    const void *_ptr;
    size_t _size;

  public:
    ConstBytes(const void *ptr, const size_t size) : _ptr(ptr), _size(size)
    {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    ConstBytes(const Bytes &bytes) // NOLINT(*-explicit-constructor)
    {
        _ptr = bytes.data();
        _size = bytes.size();
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template <typename T> ConstBytes(const T *p) // NOLINT(*-explicit-constructor)
    {
        _ptr = p;
        _size = sizeof(T);
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template <ContainerConcept T> ConstBytes(const T &c) // NOLINT(*-explicit-constructor)
    {
        _ptr = c.data();
        _size = c.size() * sizeof(typename T::value_type);
    }

    [[nodiscard]] const void *data() const noexcept
    {
        return _ptr;
    }

    [[nodiscard]] size_t size() const noexcept
    {
        return _size;
    }
};

} // namespace wsac::model