//
// Created by cve on 2025-07-01.
//

#include "Session.hpp"
#include "Host.hpp"
#include "Log.hpp"
#include "Run/State.hpp"

namespace wsac::run
{

void Session::HandleCheckpointSignal() const
{
    LogLn("checkpoint signal received");
    if (auto &layer1State = Host::Current().Get<State>().Layer1State; layer1State.Base == State::LAYER1_BASE_NONE)
    {
        // Case 1: The client requested a checkpoint. Now the system has been syncronized.
        layer1State.Base = State::LAYER1_BASE_READY;
    }
    else
    {
        // Case 2: The server has requested a checkpoint header. So we echoback checkpoint.
        _frameWriter.WriteCheckpoint();
    }
}

void Session::HandleRegularSignal()
{
    LogLn("regular signal received");
    // TODO: call singleton
}

void Session::HandleInvalidSignal() const
{
    LogLn("invalid signal received");

    // TODO: Report

    // We have to recover. So we write checkpoint.
    _frameWriter.WriteCheckpoint();
}

void Session::HandleTestSignal()
{
    LogLn("test signal received!");
}

Session::Session(io::PipeReader &reader, io::PipeWriter &writer) : _frameReader(reader), _frameWriter(writer)
{
}

void Session::Run(const std::stop_token &token) const
{
    LogLn("session started");

    // Write Test Data
    _frameWriter.WriteTestData(std::string("Echo"));

    while (!token.stop_requested())
    {
        _frameReader.ReadUntilPreamble(token);
        const auto header = _frameReader.ReadHeader(token);

        if (!header.Validate())
        {
            LogLn("header validation failed!");
            _frameWriter.WriteCheckpoint();
            Host::Current().Get<State>().Layer1State.Base = State::LAYER1_BASE_NONE;
            continue;
        }
        LogLn("header validation passed");

        switch (header.sig)
        {
        case model::Checkpoint:
            HandleCheckpointSignal();
            break;
        case model::Regular:
            HandleRegularSignal();
            break;
        case model::Test:
            HandleTestSignal();
            break;
        default:
            HandleInvalidSignal();
        }
    }

    LogLn("session ended");
}

} // namespace wsac::run