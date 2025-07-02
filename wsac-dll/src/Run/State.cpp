//
// Created by cve on 2025-06-30.
//

#include "State.hpp"

namespace wsac::run
{
State::State(std::stop_token) : Layer1State(NONE), Layer2State(NONE), Layer1PskEncryptor(PSK_LAYER1)
{
}
} // namespace wsac::run