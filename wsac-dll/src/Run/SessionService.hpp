#pragma once

#include "Include.hpp"

namespace wsac::run
{

class SessionService
{

    std::jthread _worker;

  public:
    explicit SessionService(std::stop_token st);
    ~SessionService();

    void Run(const std::stop_token& st);
};

} // namespace wsac::run