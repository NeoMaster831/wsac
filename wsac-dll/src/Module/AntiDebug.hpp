#pragma once

#include "Include.hpp"

namespace wsac::mod::antidebug
{

struct Empty {};

enum
{
    AD_OKAY,
    AD_BAD
};

cfp::Result<Empty> IsDebuggerPresent() noexcept;
cfp::Result<Empty> CheckRemoteDebuggerPresent() noexcept;
cfp::Result<Empty> CheckNtQuery() noexcept;
cfp::Result<Empty> CheckEbs() noexcept;
cfp::Result<Empty> RtlQueryProcessDebugInformation() noexcept;
cfp::Result<Empty> CheckHeapProtection() noexcept;
cfp::Result<Empty> CheckKuserSharedData() noexcept;

} // namespace wsac::mod::antidebug
