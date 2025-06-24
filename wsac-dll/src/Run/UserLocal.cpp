#include "Run/UserLocal.hpp"
#include "Log.hpp"
#include "Run/Detector.hpp"

void wsac::run::UserLocalSession::Check()
{
    for (const auto dv = Detector::GetDetectors(); const auto *d : dv)
    {
        if (d->Check())
        {
            LogLn("Rule %zu : %s", d->Id, d->Description.c_str());
        }
    }
}

void wsac::run::UserLocalSession::Start()
{
    if (running)
        return;

    running = true;
    worker = std::thread(&UserLocalSession::Loop, this);
}

void wsac::run::UserLocalSession::Stop()
{
    if (!running)
        return;

    running = false;
    if (worker.joinable())
        worker.join();
}

void wsac::run::UserLocalSession::Loop() const
{
    while (running)
    {
        Check();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}
