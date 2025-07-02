#include "Comm.hpp"

#include "Host.hpp"
#include "Log.hpp"
#include "Run/State.hpp"
#include "Sec/PskManager.hpp"

namespace
{
const std::wstring PipeName = L"WsACPipe";

HANDLE OpenPipe()
{
    const std::wstring pipeName = L"\\\\.\\pipe\\" + PipeName;

    void *pipe = CreateFileW(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        LogLn("Failed to connect to pipe, Error: %x", GetLastError());
        // TODO : Use custom exception
        throw std::exception();
    }

    return pipe;
}

void ClosePipe(void *pipe)
{
    if (!pipe)
        return;
    CloseHandle(pipe);
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