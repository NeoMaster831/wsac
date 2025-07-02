#pragma once

#include "../../Model/Vector.hpp"
#include "Include.hpp"

namespace wsac::crypto
{
using namespace wsac::model;

class ChaCha20
{
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_KEY_HANDLE hKey = nullptr;

  public:
    static constexpr size_t NonceLen = 12;
    static constexpr size_t KeyLen = 32;
    static constexpr size_t MacLen = 16;

    using Key = std::array<uint8_t, 32>;
    using Nonce = std::array<uint8_t, 12>;
    using Mac = std::array<uint8_t, 16>;

    explicit ChaCha20(const std::array<uint8_t, KeyLen> &key);
    ~ChaCha20();

    void SealBase(const Vector<uint8_t> &plain, const Nonce &nonce, const Vector<uint8_t> &authInfo, Mac &outMac,
                  Vector<uint8_t> &outCipher) const;
    void Seal(const Vector<uint8_t> &plain, const Nonce &nonce, Mac &outMac, Vector<uint8_t> &outCipher) const;
    void Open(const Vector<uint8_t> &cipher, const Nonce &nonce, Mac &mac, Vector<uint8_t> &outPlain) const;
    void Sign(const Vector<uint8_t> &message, const Nonce &nonce, Mac &outMac) const;
};
} // namespace wsac::crypto