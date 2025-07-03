#include "Host.hpp"
#include "Include.hpp"
#include "Log.hpp"
#include "Net/Comm.hpp"
#include "Run/SessionService.hpp"
#include "Run/State.hpp"

namespace wsac
{

static Host host;

bool Enable()
{
    try
    {
        host.Add<run::State>(); // First, initialize machine states.
        host.Add<net::Comm>(); // Second, initialize service communications.
        host.Add<run::SessionService>(); // Third, we start a session.
    }
    catch (std::exception& e)
    {
        LogLn("Exception during initialization: %s", e.what());
        return false;
    }

    return true;
}

void Disable()
{
    // dispose twice is intended
    host.~Host();
}
} // namespace wsac

extern "C" __declspec(dllexport) BOOLEAN Enable()
{
    return wsac::Enable() ? TRUE : FALSE;
}

extern "C" __declspec(dllexport) void Disable()
{
    wsac::Disable();
}