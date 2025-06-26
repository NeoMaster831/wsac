#include "Include.hpp"
#include "Run/Comm.hpp"
#include "Run/UserLocal.hpp"

namespace wsac
{

static run::UserLocalSession uls;

bool Enable()
{
    if (!run::comm::Initialize())
    {
        return false;
    }

    uls.Start();
    return true;
}

void Disable()
{
    uls.Stop();
    run::comm::Terminate();
}
} // namespace wsac

extern "C" __declspec(dllexport) BOOLEAN Enable()
{
    if (wsac::Enable())
    {
        return TRUE;
    }
    return FALSE;
}

extern "C" __declspec(dllexport) void Disable()
{
    wsac::Disable();
}