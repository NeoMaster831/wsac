#include "Comm.hpp"

#include "Log.hpp"

namespace
{
const std::wstring PipeName = L"WsACPipe";

HANDLE OpenPipe()
{
    const std::wstring pipeName = L"\\\\.\\pipe\\" + PipeName;

    const HANDLE pipe = CreateFileW(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (pipe == INVALID_HANDLE_VALUE)
    {
        LogLn("Failed to connect to pipe, Error: %x", GetLastError());
        // TODO : Use custom exception
        throw std::exception();
    }

    return pipe;
}

void ClosePipe(HANDLE pipe)
{
    if (!pipe)
        return;
    CloseHandle(pipe);
    pipe = nullptr;
}

} // namespace

wsac::net::CommService::CommService(std::stop_token) : _pipe(OpenPipe()), _reader(_pipe), _writer(_pipe)
{
}

wsac::net::CommService::~CommService()
{
    ClosePipe(_pipe);
}

const wsac::net::MessageReader &wsac::net::CommService::GetReader() const noexcept
{
    return _reader;
}

const wsac::net::MessageWriter &wsac::net::CommService::GetWriter() const noexcept
{
    return _writer;
}
