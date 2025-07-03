#pragma once

#include "Model/Bytes.hpp"
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

    void SealBase(ConstBytes plain, const Nonce &nonce, ConstBytes authInfo, Mac &outMac, Bytes outCipher) const;
    void Seal(ConstBytes plain, const Nonce &nonce, Mac &outMac, Bytes outCipher) const;
    void Open(ConstBytes cipher, const Nonce &nonce, Mac &mac, Bytes outPlain) const;
    void Sign(ConstBytes message, const Nonce &nonce, Mac &outMac) const;
};
} // namespace wsac::crypto