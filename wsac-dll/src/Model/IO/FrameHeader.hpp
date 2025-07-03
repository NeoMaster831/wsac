#pragma once

#include "Include.hpp"

namespace wsac::model
{
enum FrameSig : uint16_t
{
    Null = 0,
    Checkpoint = CHECKPOINT_SIGNATURE,
    Regular = REGULAR_SIGNATURE,
    Test = TEST_SIGNATURE
};

#pragma pack(push, 1)
struct FrameHeader
{
    FrameSig sig;
    uint64_t dataSize;
    std::array<uint8_t, 12> nonce{};
    std::array<uint8_t, 16> mac{};

    FrameHeader() = delete;
    FrameHeader(FrameSig sig, uint64_t size);

    [[nodiscard]] bool Validate() const;
    void Sign();

    static FrameHeader Null();
};
#pragma pack(pop)
}