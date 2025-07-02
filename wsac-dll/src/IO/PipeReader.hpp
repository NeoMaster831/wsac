#pragma once

#include "../Model/Vector.hpp"
#include "Include.hpp"

namespace wsac::io
{
class PipeReader
{
    HANDLE _pipe;

  public:
    explicit PipeReader(void *pipe);
    void Read(model::Vector<uint8_t> &v, const std::stop_token &st) const;

    template<typename T>
    void Read(T &v, const std::stop_token &st) const
    {
        model::Vector ref{
            reinterpret_cast<uint8_t*>(&v),
            sizeof(T)
        };
        Read(ref, st);
    }
};

} // namespace wsac::io