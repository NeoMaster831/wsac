#include "Host.hpp"
#include "Include.hpp"
#include "Log.hpp"
#include "Net/Comm.hpp"
#include "Run/Selector.hpp"
#include "Run/State.hpp"
#include "Sec/CryptoService.hpp"

namespace wsac
{

static Host host;

void Enable()
{
    host.Add<run::State>();
    host.Add<net::CommService>();
    host.Add<sec::CryptoService>();
    host.Add<run::Selector>();
}

void Disable()
{
    // dispose twice is intended
    host.~Host();
}
} // namespace wsac

extern "C" __declspec(dllexport) BOOLEAN Enable()
{
    try
    {
        wsac::Enable();
        return TRUE;
    }
    catch (std::exception& e)
    {
        LogLn("%s", e.what());
        return FALSE;
    }
}

extern "C" __declspec(dllexport) void Disable()
{
    wsac::Disable();
}