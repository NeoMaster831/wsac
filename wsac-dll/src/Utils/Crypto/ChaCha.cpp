//
// Created by cve on 2025-07-01.
//

#include "ChaCha.hpp"

namespace wsac::crypto
{

ChaCha20::ChaCha20(const std::array<uint8_t, KeyLen> &key)
{
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_CHACHA20_POLY1305_ALGORITHM, nullptr, 0)))
        throw ChaChaException();

    if (!BCRYPT_SUCCESS(
            BCryptGenerateSymmetricKey(hAlg, &hKey, nullptr, 0, const_cast<PUCHAR>(key.data()), key.size(), 0)))
        throw ChaChaException();
}

ChaCha20::~ChaCha20()
{
    if (hKey)
        UNREFERENCED_PARAMETER(BCryptDestroyKey(hKey));
    if (hAlg)
        UNREFERENCED_PARAMETER(BCryptCloseAlgorithmProvider(hAlg, 0));
}

void ChaCha20::SealBase(const Vector<uint8_t> &plain, const Nonce &nonce, const Vector<uint8_t> &authInfo, Mac &outMac,
                        Vector<uint8_t> &outCipher) const
{
    if (outCipher.size() != plain.size())
        throw ArgumentException("lengths of ciphertext buffer and plaintext buffer must be matched");

    NTSTATUS st = STATUS_SUCCESS;

    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
    BCRYPT_INIT_AUTH_MODE_INFO(info);

    // Nonce should not null
    info.pbNonce = const_cast<PUCHAR>(nonce.data());
    info.cbNonce = nonce.size();

    // Mac should not null, too.
    info.pbTag = outMac.data();
    info.cbTag = outMac.size();

    if (!authInfo.size() == 0)
    {
        info.pbAuthData = const_cast<PUCHAR>(authInfo.data());
        info.cbAuthData = authInfo.size();
    }

    PUCHAR pbInput;
    size_t cbInput;
    if (plain.size() == 0)
    {
        pbInput = nullptr;
        cbInput = 0;
    }
    else
    {
        pbInput = const_cast<PUCHAR>(plain.data());
        cbInput = plain.size();
    }

    PUCHAR pbOutput;
    size_t cbOutput;
    if (outCipher.size() == 0)
    {
        pbOutput = nullptr;
        cbOutput = 0;
    }
    else
    {
        pbOutput = outCipher.data();
        cbOutput = outCipher.size();
    }

    ULONG outLen = 0;
    st = BCryptEncrypt(hKey, pbInput, cbInput, &info, nullptr, 0, pbOutput, cbOutput, &outLen, 0);

    if (!BCRYPT_SUCCESS(st) || outLen != plain.size())
        throw ChaChaException();
}

void ChaCha20::Seal(const Vector<uint8_t> &plain, const Nonce &nonce, Mac &outMac, Vector<uint8_t> &outCipher) const
{
    return SealBase(plain, nonce, {nullptr, 0}, outMac, outCipher);
}

void ChaCha20::Open(const Vector<uint8_t> &cipher, const Nonce &nonce, Mac &mac, Vector<uint8_t> &outPlain) const
{
    if (outPlain.size() != cipher.size())
        throw ArgumentException("lengths of ciphertext buffer and plaintext buffer must be matched");

    NTSTATUS st = STATUS_SUCCESS;

    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
    BCRYPT_INIT_AUTH_MODE_INFO(info);

    info.pbNonce = const_cast<PUCHAR>(nonce.data());
    info.cbNonce = nonce.size();

    info.pbTag = mac.data();
    info.cbTag = mac.size();

    ULONG outLen = 0;
    st = BCryptDecrypt(hKey, const_cast<PUCHAR>(cipher.data()), cipher.size(), &info, nullptr, 0, outPlain.data(),
                       outPlain.size(), &outLen, 0);
    if (!BCRYPT_SUCCESS(st) || outLen != cipher.size())
        throw ChaChaException();
}

void ChaCha20::Sign(const Vector<uint8_t> &message, const Nonce &nonce, Mac &outMac) const
{
    Vector<uint8_t> buffer{nullptr, 0};
    SealBase(buffer, nonce, message, outMac, buffer);
}
} // namespace wsac::crypto