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

void ChaCha20::SealBase(const ConstBytes plain, const Nonce &nonce, const ConstBytes authInfo, Mac &outMac,
                        Bytes outCipher) const
{
    if (outCipher.size() != plain.size())
        throw ArgumentException("lengths of ciphertext buffer and plaintext buffer must be matched");

    NTSTATUS st = STATUS_SUCCESS;

    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {};
    BCRYPT_INIT_AUTH_MODE_INFO(info);

    info.pbNonce = const_cast<PUCHAR>(nonce.data());
    info.cbNonce = nonce.size();

    info.pbTag = outMac.data();
    info.cbTag = outMac.size();

    info.pbAuthData = (PUCHAR)authInfo.data();
    info.cbAuthData = authInfo.size();

    ULONG outLen = 0;
    st = BCryptEncrypt(hKey, (PUCHAR)plain.data(), plain.size(), &info, nullptr, 0,
                       static_cast<uint8_t *>(outCipher.data()), outCipher.size(), &outLen, 0);

    if (!BCRYPT_SUCCESS(st) || outLen != plain.size())
        throw ChaChaException();
}

void ChaCha20::Seal(const ConstBytes plain, const Nonce &nonce, Mac &outMac, const Bytes outCipher) const
{
    return SealBase(plain, nonce, {nullptr, 0}, outMac, outCipher);
}

void ChaCha20::Open(const ConstBytes cipher, const Nonce &nonce, Mac &mac, Bytes outPlain) const
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
    st = BCryptDecrypt(hKey, (PUCHAR)cipher.data(), cipher.size(), &info, nullptr, 0, (PUCHAR)outPlain.data(),
                       outPlain.size(), &outLen, 0);
    if (!BCRYPT_SUCCESS(st) || outLen != cipher.size())
        throw ChaChaException();
}

void ChaCha20::Sign(const ConstBytes message, const Nonce &nonce, Mac &outMac) const
{
    const Bytes buffer{nullptr, 0};
    SealBase(buffer, nonce, message, outMac, buffer);
}

} // namespace wsac::crypto