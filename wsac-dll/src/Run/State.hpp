#pragma once

#include "Utils/Crypto/ChaCha.hpp"
#include "Include.hpp"

namespace wsac::run
{

struct State
{
    static constexpr uint64_t NONE = 0;

    static constexpr uint64_t LAYER1_BASE_READY = 1;
    static constexpr uint64_t LAYER1_BASE_NONE = 0;
    union {
        uint64_t Value;
        struct
        {
            uint64_t Unused : 62;
            uint64_t ShouldReadWithoutPreamble : 1;
            uint64_t Base : 1;
        };
    } Layer1State;

    union {
        uint64_t Value;
        struct
        {
            uint64_t Unused : 62;
            uint64_t Base : 2;
        };
    } Layer2State;

    crypto::ChaCha20 Layer1PskEncryptor;

    explicit State(std::stop_token);
};

} // namespace wsac::run
