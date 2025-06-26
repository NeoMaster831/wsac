#include "Run/UserLocal.hpp"

#include "Comm.hpp"
#include "Log.hpp"

void wsac::run::UserLocalSession::Check()
{
    std::vector<uint8_t> body;
    for (const auto *d : cfp::Register::All())
    {
        auto status = (*d)(body);

        // TODO : replace request id
        if (const bool result = comm::ReportBuilder().Header({0, d->getId(), status}).Body(body).Send(); !result)
        {
            LogLn("Couldn't send ID: %zu", d->getId());
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

        // TODO: Fix
        for (auto i = 0; i < 10 && running; ++i)
            std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}