#pragma once

#include <exception>

#define MAKE_CUSTOM_EXCEPTION(name, description)                                                                       \
    struct name final : std::exception                                                                                 \
    {                                                                                                                  \
        const char *what() const noexcept override                                                                     \
        {                                                                                                              \
            return #description;                                                                                       \
        }                                                                                                              \
    };

#define MAKE_CUSTOM_ARGUMENT_EXCEPTION(name)                                                                           \
    struct name final : std::exception                                                                                 \
    {                                                                                                                  \
        std::string _msg;                                                                                              \
                                                                                                                       \
      public:                                                                                                          \
        explicit name(const std::string &msg) : _msg(msg)                                                              \
        {                                                                                                              \
        }                                                                                                              \
        const char *what() const noexcept override                                                                     \
        {                                                                                                              \
            return _msg.c_str();                                                                                       \
        }                                                                                                              \
    }

namespace wsac
{
namespace crypto
{

MAKE_CUSTOM_ARGUMENT_EXCEPTION(ArgumentException);
MAKE_CUSTOM_EXCEPTION(ChaChaException, ChaCha20 Exception);
MAKE_CUSTOM_EXCEPTION(RandomException, Random Exception);

} // namespace crypto

namespace io
{

MAKE_CUSTOM_EXCEPTION(PipeOpenFailedException, Pipe open failed exception);
MAKE_CUSTOM_EXCEPTION(PipeReadException, Pipe read exception);
MAKE_CUSTOM_EXCEPTION(PipeWriteException, Pipe write exception);

} // namespace io

MAKE_CUSTOM_EXCEPTION(StopTokenRequestedException, Stop token requested exception);

} // namespace wsac