#pragma once

#include "Include.hpp"
#include "Model/Bytes.hpp"

namespace wsac::io
{
class PipeReader
{
    HANDLE _pipe;

  public:
    explicit PipeReader(void *pipe);
    void Read(model::Bytes v, const std::stop_token &st) const;

    template<typename T>
    void Read(T &v, const std::stop_token &st) const
    {
        const model::Bytes ref{
            reinterpret_cast<uint8_t*>(&v),
            sizeof(T)
        };
        Read(ref, st);
    }
};

} // namespace wsac::io