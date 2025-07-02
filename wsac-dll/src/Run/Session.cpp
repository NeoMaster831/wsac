//
// Created by cve on 2025-07-01.
//

#include "Session.hpp"

#include "Host.hpp"
#include "Log.hpp"
#include "State.hpp"

namespace wsac::run
{

void Session::HandleCheckpointSignal()
{
    if (const auto &layer1State = Host::Get<State>().Layer1State; layer1State.Base == State::LAYER1_BASE_NONE)
    {
        // Case 1: The client requested a checkpoint. Now the system has been syncronized.
        Host::Get<State>().Layer1State.Base = State::LAYER1_BASE_READY;
    }
    else
    {
        // Case 2: The server has requested a checkpoint header. So we echoback checkpoint.
        _frameWriter.WriteCheckpoint();
    }
}

void Session::HandleRegularSignal()
{
    LogLn("Wowie handle handle");
}

void Session::HandleInvalidSignal() const
{
    // TODO: Report

    // We have to recover. So we write checkpoint.
    _frameWriter.WriteCheckpoint();
}

Session::Session(io::PipeReader &reader, io::PipeWriter &writer) : _frameReader(reader), _frameWriter(writer)
{
}

void Session::Run(const std::stop_token &token)
{
    while (token.stop_requested())
    {
        _frameReader.ReadUntilPreamble(token);
        const auto header = _frameReader.ReadHeader(token);

        if (!header.Validate())
        {
            _frameWriter.WriteCheckpoint();
            Host::Get<State>().Layer1State.Base = State::LAYER1_BASE_NONE;
            continue;
        }

        switch (header.sig)
        {
        case CHECKPOINT_SIGNATURE:
            HandleCheckpointSignal();
            break;
        case REGULAR_SIGNATURE:
            HandleRegularSignal();
            break;
        default:
            HandleInvalidSignal();
        }
    }
}

} // namespace wsac::run