#include "Cryptography.hpp"

#include "Host.hpp"

class CryptoException final : public std::exception
{
    std::string _msg;

public:
    explicit CryptoException(std::string msg) : _msg(std::move(msg)) {}

    const char* what() const noexcept override
    {
        return _msg.c_str();
    }
};

wsac::sec::Cryptography::Cryptography()
    : _alg(nullptr), _key(nullptr), _encAlg(nullptr)
{
    NTSTATUS st = STATUS_SUCCESS;
    ULONG sz = 0;

    st = BCryptOpenAlgorithmProvider(&_alg, BCRYPT_ECDH_P256_ALGORITHM, nullptr, 0);
    if (!BCRYPT_SUCCESS(st))
        throw CryptoException("BCryptOpenAlgorithmProvider failed"); // TODO : custom exception

    st = BCryptGenerateKeyPair(_alg, &_key, 256, 0);
    if (!BCRYPT_SUCCESS(st))
        throw CryptoException("BCryptGenerateKeyPair failed"); // TODO : custom exception

    st = BCryptFinalizeKeyPair(_key, 0);
    if (!BCRYPT_SUCCESS(st))
        throw CryptoException("BCryptFinalizeKeyPair failed"); // TODO : custom exception

    st = BCryptExportKey(_key, nullptr, BCRYPT_ECCPUBLIC_BLOB, nullptr, 0, &sz, 0);
    if (!BCRYPT_SUCCESS(st))
        throw CryptoException("BCryptExportKey failed"); // TODO : custom exception

    _pubKey.resize(sz);

    st = BCryptExportKey(_key, nullptr, BCRYPT_ECCPUBLIC_BLOB, _pubKey.data(), sz, &sz, 0);
    if (!BCRYPT_SUCCESS(st))
        throw CryptoException("BCryptExportKey failed"); // TODO : custom exception

    st = BCryptOpenAlgorithmProvider(&_encAlg, BCRYPT_CHACHA20_POLY1305_ALGORITHM, nullptr, 0);
    if (!BCRYPT_SUCCESS(st))
        throw CryptoException("BCryptOpenAlgorithmProvider failed"); // TODO : custom exception
}

wsac::sec::Cryptography::~Cryptography()
{
    if (_key)
        UNREFERENCED_PARAMETER(BCryptDestroyKey(_key));

    if (_alg)
        UNREFERENCED_PARAMETER(BCryptCloseAlgorithmProvider(_alg, 0));

    if (_encAlg)
        UNREFERENCED_PARAMETER(BCryptCloseAlgorithmProvider(_encAlg, 0));

    SecureZeroMemory(_shared.data(), _shared.size());
    SecureZeroMemory(_pubKey.data(), _pubKey.size());
}

const std::vector<uint8_t> &wsac::sec::Cryptography::PublicKey() const
{
    return _pubKey;
}

bool wsac::sec::Cryptography::InstallPairKey(const std::vector<uint8_t> &key)
{
    BCRYPT_KEY_HANDLE srvKey = nullptr;
    BCRYPT_SECRET_HANDLE secret = nullptr;
    NTSTATUS st = STATUS_SUCCESS;

    st = BCryptImportKeyPair(_alg, nullptr, BCRYPT_ECCPUBLIC_BLOB, &srvKey, const_cast<PUCHAR>(key.data()), static_cast<ULONG>(key.size()),
                             0);
    if (!BCRYPT_SUCCESS(st))
        return false;

    st = BCryptSecretAgreement(_key, srvKey, &secret, 0);
    if (!BCRYPT_SUCCESS(st))
    {
        UNREFERENCED_PARAMETER(BCryptDestroyKey(srvKey));
        return false;
    }

    ULONG len = 0;
    st = BCryptDeriveKey(secret, BCRYPT_KDF_RAW_SECRET, nullptr, nullptr, 0, &len, 0);
    if (!BCRYPT_SUCCESS(st) || len == 0)
    {
        UNREFERENCED_PARAMETER(BCryptDestroySecret(secret));
        UNREFERENCED_PARAMETER(BCryptDestroyKey(srvKey));
        return false;
    }

    _shared.resize(len);
    st = BCryptDeriveKey(secret, BCRYPT_KDF_RAW_SECRET, nullptr, _shared.data(), len, &len, 0);

    UNREFERENCED_PARAMETER(BCryptDestroySecret(secret));
    UNREFERENCED_PARAMETER(BCryptDestroyKey(srvKey));

    return BCRYPT_SUCCESS(st);
}

bool wsac::sec::Cryptography::Seal(const std::vector<uint8_t> &plain, std::vector<uint8_t> &sealed)
{
    auto& self = Host::Current().Get<Cryptography>();

    NTSTATUS st = STATUS_SUCCESS;
    BCRYPT_KEY_HANDLE symKey = nullptr;

    st = BCryptGenerateSymmetricKey(self._encAlg, &symKey, nullptr, 0, self._shared.data(),
                                    static_cast<ULONG>(self._shared.size()), 0);
    if (!BCRYPT_SUCCESS(st))
        return false;

    std::vector<uint8_t> nonce(NonceLen);
    st = BCryptGenRandom(nullptr, nonce.data(), static_cast<ULONG>(nonce.size()), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(st))
    {
        UNREFERENCED_PARAMETER(BCryptDestroyKey(symKey));
        return false;
    }

    std::vector<uint8_t> tag(TagLen);
    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {0};
    BCRYPT_INIT_AUTH_MODE_INFO(info);

    info.pbNonce = nonce.data();
    info.cbNonce = static_cast<ULONG>(nonce.size());
    info.pbTag = tag.data();
    info.cbTag = static_cast<ULONG>(tag.size());

    std::vector<uint8_t> cipher(plain.size());
    ULONG outLen = 0;
    st = BCryptEncrypt(symKey, const_cast<PUCHAR>(plain.data()), static_cast<ULONG>(plain.size()), &info, nullptr, 0, cipher.data(),
                       static_cast<ULONG>(cipher.size()), &outLen, 0);

    UNREFERENCED_PARAMETER(BCryptDestroyKey(symKey));

    if (!BCRYPT_SUCCESS(st) || outLen != plain.size())
        return false;

    sealed.clear();
    sealed.reserve(nonce.size() + tag.size() + cipher.size());
    sealed.insert(sealed.end(), nonce.begin(), nonce.end());
    sealed.insert(sealed.end(), tag.begin(), tag.end());
    sealed.insert(sealed.end(), cipher.begin(), cipher.end());

    SecureZeroMemory(nonce.data(), nonce.size());
    SecureZeroMemory(tag.data(), tag.size());
    SecureZeroMemory(cipher.data(), cipher.size());

    return true;
}

bool wsac::sec::Cryptography::Open(const std::vector<uint8_t> &sealed, std::vector<uint8_t> &plain)
{
    auto& self = Host::Current().Get<Cryptography>();

    const size_t cipherLen = sealed.size() - NonceLen - TagLen;

    const PUCHAR nonce = const_cast<PUCHAR>(sealed.data());
    const PUCHAR tag = nonce + NonceLen;
    const uint8_t *cipher = tag + TagLen;

    BCRYPT_KEY_HANDLE symKey = nullptr;
    NTSTATUS st = STATUS_SUCCESS;

    st = BCryptGenerateSymmetricKey(self._encAlg, &symKey, nullptr, 0, self._shared.data(),
                                    static_cast<ULONG>(self._shared.size()), 0);
    if (!BCRYPT_SUCCESS(st))
        return false;

    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO info = {0};
    BCRYPT_INIT_AUTH_MODE_INFO(info);

    info.pbNonce = nonce;
    info.cbNonce = static_cast<ULONG>(NonceLen);
    info.pbTag = tag;
    info.cbTag = static_cast<ULONG>(TagLen);

    plain.resize(cipherLen);
    ULONG outLen = 0;

    st = BCryptDecrypt(symKey, const_cast<PUCHAR>(cipher), static_cast<ULONG>(cipherLen), &info, nullptr, 0,
                       plain.data(), static_cast<ULONG>(plain.size()), &outLen, 0);

    UNREFERENCED_PARAMETER(BCryptDestroyKey(symKey));

    if (!BCRYPT_SUCCESS(st) || outLen != cipherLen)
    {
        plain.clear();
        return false;
    }

    return true;
}
