#pragma once

#include "Include.hpp"

namespace wsac::run
{

class UserLocalSession
{

    std::thread worker;
    std::atomic<bool> running;

    void Loop() const;

  public:
    static void Check();
    void Start();
    void Stop();

    explicit UserLocalSession() = default;
    ~UserLocalSession()
    {
        Stop();
    }
};
} // namespace wsac::run