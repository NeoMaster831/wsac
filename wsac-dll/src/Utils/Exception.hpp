#pragma once

#include <exception>

class ArgumentException final : std::exception
{
    std::string _msg;

  public:
    explicit ArgumentException(const std::string &msg) : _msg(msg)
    {
    }

    const char *what() const noexcept override
    {
        return _msg.c_str();
    }
};

struct ChaChaException final : std::exception
{
    const char *what() const noexcept override
    {
        return "ChaCha20 exception";
    }
};

struct RandomException final : std::exception
{
    const char *what() const noexcept override
    {
        return "Random exception";
    }
};

struct PipeReadException final : std::exception
{
    const char *what() const noexcept override
    {
        return "Pipe read exception";
    }
};

struct PipeWriteException final : std::exception
{
    const char *what() const noexcept override
    {
        return "Pipe write exception";
    }
};

struct StopTokenRequestedException final : std::exception
{
    const char *what() const noexcept override
    {
        return "Stop token requested exception";
    }
};