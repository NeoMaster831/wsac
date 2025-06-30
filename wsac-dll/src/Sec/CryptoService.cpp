#include "CryptoService.hpp"

#include "Cryptography.hpp"
#include "Host.hpp"
#include "Run/State.hpp"

namespace
{

wsac::sec::Cryptography &RegisterCryptography()
{
    auto &host = wsac::Host::Current();
    host.Add<wsac::sec::Cryptography>();
    return host.Get<wsac::sec::Cryptography>();
}

} // namespace

namespace wsac::sec
{

CryptoService::CryptoService()
{
    // send key info
}

void CryptoService::InstallServerKey(const std::vector<uint8_t> &key)
{
    auto &crypto = RegisterCryptography();
    crypto.InstallPairKey(key);
    Host::Current().Get<run::State>().KeyExchanged = true;
}

} // namespace wsac::sec
