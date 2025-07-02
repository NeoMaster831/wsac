#include "Random.hpp"

namespace wsac::crypto
{

void Random::Fill(uint8_t *data, const size_t size)
{
    if (const NTSTATUS st = BCryptGenRandom(nullptr, data, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG); !BCRYPT_SUCCESS(st))
    {
        throw RandomException();
    }
}

} // namespace wsac::crypto