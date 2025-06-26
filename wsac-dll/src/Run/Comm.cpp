#include "Run/Comm.hpp"

#include "Log.hpp"

namespace
{
const std::wstring PipeName = L"WsACPipe";
auto PipeHandle = INVALID_HANDLE_VALUE;

bool SendToPipe(const void *p, const size_t size)
{
    DWORD written;
    if (!WriteFile(PipeHandle, p, size, &written, nullptr) || written != size)
    {
        LogLn("Couldn't write pipe, Error: %x", GetLastError());
        return false;
    }

    // We do not receive echobacks for more security

    return true;
}
} // namespace

wsac::run::comm::ReportHeader::ReportHeader(const uint64_t requestId, const uint64_t id, const int32_t status)
    : RequestId(requestId), Id(id), Status(status), Timestamp()
{
    using namespace std::chrono;
    Timestamp = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

bool wsac::run::comm::ReportBuilder::Send() const noexcept
{
    if (!_header)
        return false;

    if (!SendToPipe(&_header, sizeof _header))
        return false;

    const int size = _body ? std::min(_body->size(), static_cast<size_t>(INT32_MAX)) : 0;
    if (!SendToPipe(&size, sizeof size))
        return false;

    if (_body && !SendToPipe(_body->data(), size))
        return false;

    return true;
}

bool wsac::run::comm::Initialize() noexcept
{
    const std::wstring pipeName = L"\\\\.\\pipe\\" + PipeName;

    PipeHandle = CreateFileW(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (PipeHandle == INVALID_HANDLE_VALUE)
    {
        LogLn("Failed to connect to pipe, Error: %x", GetLastError());
        return false;
    }

    return true;
}

void wsac::run::comm::Terminate() noexcept
{
    if (PipeHandle == nullptr)
        return;

    if (!ReportBuilder().Header({0, PEACEFUL_EXIT, 0}).Send())
    {
        LogLn("Peaceful closing of pipe failed!");
    }

    CloseHandle(PipeHandle);
    PipeHandle = nullptr;
}
