#include "Module/AntiDebug.hpp"
#include "Log.hpp"

cfp::Result<wsac::mod::antidebug::Empty> wsac::mod::antidebug::IsDebuggerPresent() noexcept
{
    auto status = ::IsDebuggerPresent() ? AD_BAD : AD_OKAY;
    return {status, std::nullopt};
}

cfp::Result<wsac::mod::antidebug::Empty> wsac::mod::antidebug::CheckRemoteDebuggerPresent() noexcept
{
    BOOL result = FALSE;
    if (!::CheckRemoteDebuggerPresent(GetCurrentProcess(), &result))
    {
        LogLn("CheckRemoteDebuggerPresent returned FALSE! Ignoring...");
        return {AD_OKAY, std::nullopt};
    }

    return {result ? AD_BAD : AD_OKAY, std::nullopt};
}

cfp::Result<wsac::mod::antidebug::Empty> wsac::mod::antidebug::CheckNtQuery() noexcept
{

    ULONG retLen = 0;
    UNREFERENCED_PARAMETER(retLen);

    HANDLE hDbgObj = nullptr;
    if (!NT_SUCCESS(
            NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugPort, &hDbgObj, sizeof(hDbgObj), &retLen)))
    {
        LogLn("Couldn't process NtQueryInformationProcess with ProcessDebugPort!");
    }

    // ProcessDebugPort should be 0.
    else if (hDbgObj != nullptr)
    {
        LogLn("ProcessDebugPort check failed");
        return {AD_BAD, std::nullopt};
    }

    const NTSTATUS debugObjectStatus =
        NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugObjectHandle, &hDbgObj, sizeof(hDbgObj), &retLen);
    if (!NT_SUCCESS(debugObjectStatus))
    {
        if (debugObjectStatus != STATUS_PORT_NOT_SET)
        {
            LogLn("DebugObjectStatus code check failed");
            return {AD_BAD, std::nullopt};
        }
    }
    // ProcessDebugObjectHandle should be NULL.
    else if (hDbgObj != nullptr)
    {
        LogLn("ProcessDebugObjectHandle not null");
        return {AD_BAD, std::nullopt};
    }

    ULONG flags = 0;
    if (!NT_SUCCESS(NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugFlags, &flags, sizeof(flags), &retLen)))
    {
        LogLn("Couldn't process NtQueryInformationProcess with ProcessDebugFlags!");
    }

    // ProcessDebugFlags should be 1.
    else if (flags != 1)
    {
        LogLn("ProcessDebugFlags not 1");
        return {AD_BAD, std::nullopt};
    }

    // Now all tests passed.
    return {AD_OKAY, std::nullopt};
}

cfp::Result<wsac::mod::antidebug::Empty> wsac::mod::antidebug::CheckEbs() noexcept
{

    const auto peb = NtCurrentPeb();

    if (peb->BeingDebugged)
    {
        LogLn("BeingDebugged is true");
        return {AD_BAD, std::nullopt};
    }

    if (peb->NtGlobalFlag & (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS))
    {
        LogLn("NtGlobalFlag check failed");
        return {AD_BAD, std::nullopt};
    }

    return {AD_OKAY, std::nullopt};
}

cfp::Result<wsac::mod::antidebug::Empty> wsac::mod::antidebug::RtlQueryProcessDebugInformation() noexcept
{

    const auto buf = RtlCreateQueryDebugBuffer(0, FALSE);
    if (!buf)
    {
        LogLn("RtlCreateQueryDebugBuffer returned NULL, ignoring...");
        return {AD_OKAY, std::nullopt};
    }

    if (!NT_SUCCESS(
            ::RtlQueryProcessDebugInformation(reinterpret_cast<HANDLE>(static_cast<UINT_PTR>(GetCurrentProcessId())),
                                              RTL_QUERY_PROCESS_HEAP_SUMMARY | RTL_QUERY_PROCESS_HEAP_ENTRIES, buf)))
    {
        LogLn("RtlQueryProcessDebugInformation returned fail status, ignoring...");
        return {AD_OKAY, std::nullopt};
    }

    if (const ULONG check = static_cast<PRTL_PROCESS_HEAPS_V2>(buf->Heaps)->Heaps[0].Flags; check & ~HEAP_GROWABLE)
    {
        LogLn("Heap check failed");
        return {AD_BAD, std::nullopt};
    }

    return {AD_OKAY, std::nullopt};
}

cfp::Result<wsac::mod::antidebug::Empty> wsac::mod::antidebug::CheckHeapProtection() noexcept
{

    PROCESS_HEAP_ENTRY entry = {};
    do
    {
        if (!HeapWalk(GetProcessHeap(), &entry))
        {
            LogLn("HeapWalk failed, ignoring...");
            return {AD_OKAY, std::nullopt};
        }
    } while (entry.wFlags != PROCESS_HEAP_ENTRY_BUSY);

    if (void *const overlapped = static_cast<PBYTE>(entry.lpData) + entry.cbData;
        *static_cast<PDWORD>(overlapped) == 0xABABABAB)
    {
        LogLn("HeapProtection failed");
        return {AD_BAD, std::nullopt};
    }

    return {AD_OKAY, std::nullopt};
}

cfp::Result<wsac::mod::antidebug::Empty> wsac::mod::antidebug::CheckKuserSharedData() noexcept
{
    if (const auto sharedData = USER_SHARED_DATA; sharedData->KdDebuggerEnabled & 0x3)
    {
        LogLn("Kd Debugger Enabled!!!");
        return {AD_BAD, std::nullopt};
    }

    return {AD_OKAY, std::nullopt};
}

namespace wsac::mod::antidebug
{
CFP_REG(IS_DEBUGGER_PRESENT_ID, IsDebuggerPresent, "IsDebuggerPresent");
CFP_REG(REMOTE_DEBUGGER_PRESENT_ID, CheckRemoteDebuggerPresent, "CheckRemoteDebuggerPresent");
CFP_REG(NTQUERY_ID, CheckNtQuery, "CheckNtQuery");
CFP_REG(EBS_ID, CheckEbs, "CheckEbs");
CFP_REG(RTL_QUERY_HEAP_ID, RtlQueryProcessDebugInformation, "RtlQueryProcessDebugInformation");
CFP_REG(MANUAL_CHECK_HEAP_PROTECTION, CheckHeapProtection, "CheckHeapProtection");
CFP_REG(CHECK_KUSER_SHARED_USER, CheckKuserSharedData, "KUserSharedData");
} // namespace wsac::mod::antidebug