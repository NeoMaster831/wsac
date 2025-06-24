#include "Module/AntiDebug.hpp"
#include "Log.hpp"
#include "Run/Detector.hpp"

bool wsac::mod::antidebug::IsDebuggerPresent() noexcept
{
    return ::IsDebuggerPresent();
}

bool wsac::mod::antidebug::CheckRemoteDebuggerPresent() noexcept
{
    BOOL result = FALSE;
    if (!::CheckRemoteDebuggerPresent(GetCurrentProcess(), &result))
    {
        LogLn("CheckRemoteDebuggerPresent returned FALSE! Ignoring...");
        return false;
    }
    return result == TRUE;
}

bool wsac::mod::antidebug::CheckNtQuery() noexcept
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
        return true;
    }

    const NTSTATUS debugObjectStatus =
        NtQueryInformationProcess(GetCurrentProcess(), ProcessDebugObjectHandle, &hDbgObj, sizeof(hDbgObj), &retLen);
    if (!NT_SUCCESS(debugObjectStatus))
    {
        if (debugObjectStatus != STATUS_PORT_NOT_SET)
        {
            LogLn("DebugObjectStatus code check failed");
            return true;
        }
    }
    // ProcessDebugObjectHandle should be NULL.
    else if (hDbgObj != nullptr)
    {
        LogLn("ProcessDebugObjectHandle not null");
        return true;
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
        return true;
    }

    // Now all tests passed.
    return false;
}

bool wsac::mod::antidebug::CheckEbs() noexcept
{

    const auto peb = NtCurrentPeb();

    if (peb->BeingDebugged)
    {
        LogLn("BeingDebugged is true");
        return true;
    }

    if (peb->NtGlobalFlag & (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS))
    {
        LogLn("NtGlobalFlag check failed");
        return true;
    }

    return false;
}

bool wsac::mod::antidebug::RtlQueryProcessDebugInformation() noexcept
{

    const auto buf = RtlCreateQueryDebugBuffer(0, FALSE);
    if (!buf)
    {
        LogLn("RtlCreateQueryDebugBuffer returned NULL, ignoring...");
        return false;
    }

    if (!NT_SUCCESS(
            ::RtlQueryProcessDebugInformation(reinterpret_cast<HANDLE>(static_cast<UINT_PTR>(GetCurrentProcessId())),
                                              RTL_QUERY_PROCESS_HEAP_SUMMARY | RTL_QUERY_PROCESS_HEAP_ENTRIES, buf)))
    {
        LogLn("RtlQueryProcessDebugInformation returned fail status, ignoring...");
        return false;
    }

    if (const ULONG check = static_cast<PRTL_PROCESS_HEAPS_V2>(buf->Heaps)->Heaps[0].Flags; check & ~HEAP_GROWABLE)
    {
        LogLn("Heap check failed");
        return true;
    }

    return false;
}

bool wsac::mod::antidebug::CheckHeapProtection() noexcept
{

    PROCESS_HEAP_ENTRY entry = {};
    do
    {
        if (!HeapWalk(GetProcessHeap(), &entry))
        {
            LogLn("HeapWalk failed, ignoring...");
            return false;
        }
    } while (entry.wFlags != PROCESS_HEAP_ENTRY_BUSY);

    if (void *const overlapped = static_cast<PBYTE>(entry.lpData) + entry.cbData;
        *static_cast<PDWORD>(overlapped) == 0xABABABAB)
    {
        LogLn("HeapProtection failed");
        return true;
    }

    return false;
}

bool wsac::mod::antidebug::CheckKuserSharedData() noexcept
{
    if (const auto sharedData = USER_SHARED_DATA; sharedData->KdDebuggerEnabled & 0x3)
    {
        LogLn("Kd Debugger Enabled!!!");
        return true;
    }

    return false;
}

namespace wsac::mod::antidebug
{
DECL_DETECTOR(IsDebuggerPresent, "IsDebuggerPresent");
DECL_DETECTOR(CheckRemoteDebuggerPresent, "CheckRemoteDebuggerPresent");
DECL_DETECTOR(CheckNtQuery, "CheckNtQuery");
DECL_DETECTOR(CheckEbs, "CheckEbs");
DECL_DETECTOR(RtlQueryProcessDebugInformation, "RtlQueryProcessDebugInformation");
DECL_DETECTOR(CheckHeapProtection, "CheckHeapProtection");
DECL_DETECTOR(CheckKuserSharedData, "KUserSharedData");
} // namespace wsac::mod::antidebug