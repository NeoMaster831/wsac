#pragma once

#include "Include.hpp"

namespace wsac::crypto
{
class Random
{
    static void Fill(uint8_t *data, size_t size);

  public:
    template <typename T> static void Fill(T &&data)
    {
        Fill(data.data(), data.size());
    }
};
} // namespace wsac::crypto