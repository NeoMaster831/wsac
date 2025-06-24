#pragma once

namespace wsac::mod::antidebug
{

bool IsDebuggerPresent() noexcept;
bool CheckRemoteDebuggerPresent() noexcept;
bool CheckNtQuery() noexcept;
bool CheckEbs() noexcept;
bool RtlQueryProcessDebugInformation() noexcept;
bool CheckHeapProtection() noexcept;
bool CheckKuserSharedData() noexcept;

} // namespace wsac::mod::antidebug
