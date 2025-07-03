//
// Created by cve on 2025-07-01.
//

#include "SessionService.hpp"

#include "Host.hpp"
#include "Log.hpp"
#include "Net/Comm.hpp"
#include "Session.hpp"

namespace wsac::run
{

SessionService::SessionService(std::stop_token st) // NOLINT(*-unnecessary-value-param)
    : _worker(&SessionService::Run, this, st)
{
}

SessionService::~SessionService() = default;

// ReSharper disable once CppMemberFunctionMayBeStatic
void SessionService::Run(const std::stop_token &st) // NOLINT(*-convert-member-functions-to-static)
{
    try
    {
        auto &currentComm = Host::Current().Get<net::Comm>();
        Session session(currentComm.GetReader(), currentComm.GetWriter());
        session.Run(st);
    }
    catch (StopTokenRequestedException &)
    {
        LogLn("Stop token requested exception, skipping..");
    }
    catch (std::exception &e)
    {
        LogLn("Exception occured: %s", e.what());
        LogLn("From now, the client may cannot response to the server.");
    }

    LogLn("Session gracefully terminated");
}

} // namespace wsac::run