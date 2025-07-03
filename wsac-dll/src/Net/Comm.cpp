#include "Comm.hpp"

#include "Log.hpp"

namespace
{
const std::wstring PipeName = L"WsACPipe";

HANDLE OpenPipe()
{
    LogLn("opening pipe..");
    const std::wstring pipeName = L"\\\\.\\pipe\\" + PipeName;

    void *pipe = CreateFileW(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        LogLn("Failed to connect to pipe, Error: %x", GetLastError());
        throw wsac::io::PipeOpenFailedException();
    }
    LogLn("pipe opened");
    return pipe;
}

void ClosePipe(void *pipe)
{
    LogLn("pipe closing..");
    if (!pipe)
        return;
    CloseHandle(pipe);
    LogLn("pipe closed");
}

} // namespace

wsac::net::Comm::Comm(const std::stop_token &) : _pipe(OpenPipe()), _reader(_pipe), _writer(_pipe)
{
}

wsac::net::Comm::~Comm()
{
    ClosePipe(_pipe);
}

wsac::io::PipeReader &wsac::net::Comm::GetReader()
{
    return _reader;
}

wsac::io::PipeWriter &wsac::net::Comm::GetWriter()
{
    return _writer;
}