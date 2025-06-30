#pragma once

#include "Include.hpp"

namespace wsac::sec
{

class Cryptography
{
    BCRYPT_ALG_HANDLE _alg;
    BCRYPT_KEY_HANDLE _key;
    BCRYPT_ALG_HANDLE _encAlg;

    std::vector<uint8_t> _pubKey;
    std::vector<uint8_t> _shared;

public:
    static constexpr size_t NonceLen = 12;
    static constexpr size_t TagLen = 16;

    Cryptography();
    ~Cryptography();

    const std::vector<uint8_t>& PublicKey() const;

    bool InstallPairKey(const std::vector<uint8_t> &key);

    static bool Seal(const std::vector<uint8_t> &plain, std::vector<uint8_t> &sealed);
    static bool Open(const std::vector<uint8_t> &sealed, std::vector<uint8_t> &plain);
};

} // namespace wsac::run
