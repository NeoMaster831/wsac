//
// Created by cve on 2025-07-01.
//

#include "SessionService.hpp"

namespace wsac::run
{

SessionService::SessionService(std::stop_token st) : _worker(&SessionService::Run, this, st)
{
}
SessionService::~SessionService()
{
}
void SessionService::Run(const std::stop_token &st)
{
}
} // namespace wsac::run