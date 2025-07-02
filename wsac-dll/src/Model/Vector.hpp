#pragma once

#include "Include.hpp"

namespace wsac::model
{
template <typename T> class Vector
{
    T *_ptr;
    size_t _size;

  public:
    Vector(T *ptr, const size_t size) : _ptr(ptr), _size(size)
    {
    }

    // ReSharper disable once CppNonExplicitConvertingConstructor
    template <ContainerConcept U> Vector(U &&u)
    {
        _ptr = u.data();
        _size = u.size();
    }

    template <typename U>
        requires !ContainerConcept<U>
                 explicit Vector(U & u)
    {
        _ptr = &u;
        _size = sizeof(U);
    }

    T *data() noexcept
    {
        return _ptr;
    }
    const T *data() const noexcept
    {
        return _ptr;
    }
    size_t size() const noexcept
    {
        return _size;
    }
};
} // namespace wsac::model