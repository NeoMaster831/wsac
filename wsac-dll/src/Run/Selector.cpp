#include "Selector.hpp"

#include "Sec/Cryptography.hpp"
#include "Host.hpp"
#include "Log.hpp"
#include "Net/Comm.hpp"
#include "Sec/CryptoService.hpp"
#include "State.hpp"
#include "Config.hpp"

namespace wsac::run
{

void Selector::Run(const std::stop_token &st)
{
    auto &state = Host::Current().Get<State>();

    auto &crypto = Host::Current().Get<sec::CryptoService>();

    const auto &comm = Host::Current().Get<net::CommService>();
    auto &reader = comm.GetReader();

    std::vector<uint8_t> sealed;
    std::vector<uint8_t> plain;

    auto Recover = [&]
    {
        LogLn("Bad payload received, recovering...");
        if (reader.RecoverDueMagicByte(st))
            return false;
        LogLn("Recovered");
        return true;
    };

    LogLn("Selector started running");

    while (!st.stop_requested())
    {

        if (!state.KeyExchanged)
        {
            uint64_t signature;
            if (!reader.Read(&signature, sizeof signature, st))
            {
                // exception
            }

            if (signature != KEY_EXCHANGE_SIGNATURE)
            {
                Recover();
            }

            int32_t size;
            if (!reader.Read(&size, sizeof size, st))
            {
                // exception
            }

            int32_t keySize;
            if (!reader.Read(&keySize, sizeof keySize, st))
            {
                // exception
            }

            if (size != sizeof keySize + keySize)
            {
                // exception
            }

            std::vector<uint8_t> key;
            if (!reader.Read(key.data(), keySize, st))
            {
                // exception
            }

            crypto.InstallServerKey(key);
        }
        else
        {
            std::array<uint8_t, 12> nonce;
            if (!reader.Read(nonce.data(), nonce.size(), st))
            {
                // exception
            }

            std::array<uint8_t, 16> tag;
            if (!reader.Read(tag.data(), tag.size(), st))
            {
                // exception
            }

            struct
            {
                uint64_t signature;
                uint32_t size;
            } header;

            if (!reader.Read(&header, sizeof header, st))
            {
                // exception
            }

            if (header.signature != PAYLOAD_SIGNATURE)
            {
                // exception
            }

            std::vector<uint8_t> body;
            if (!reader.Read(body.data(), header.size, st))
            {
                // exception
            }

            auto it = _selectables.find(header.signature);
            if (it == _selectables.end())
            {
                Recover();
                break;
            }

            it->second.Process(plain.data() + plainHeaderLen);

        }





        if (!crypto.Exchanged())
        {
            // fuck you
        }
        uint64_t sig;

        // Signature (8) + dataSize (4)
        constexpr size_t plainHeaderLen = sizeof(Signature) + sizeof(int);

        // Nonce (12) + Tag (16) + Signature (8) + dataSize (4)
        constexpr size_t sealedHeaderLen =
            sec::Cryptography::NonceLen + sec::Cryptography::TagLen + sizeof(Signature) + sizeof(int);

        // First, try to decrypt signature and dataSize first
        sealed.resize(sealedHeaderLen);
        if (!reader.Read(sealed.data(), sealedHeaderLen, st))
            break;

        crypto.Open(sealed, plain);
        if (plain.size() != plainHeaderLen && !Recover())
            break;

        sig = *reinterpret_cast<Signature *>(plain.data());
        const int dataSize = *reinterpret_cast<int *>(plain.data() + sizeof(Signature));

        // This time we try to decrypt whole data
        sealed.resize(sealedHeaderLen + dataSize);
        if (!reader.Read(sealed.data() + sealedHeaderLen, dataSize, st))
            break;

        crypto.Open(sealed, plain);
        if (plain.size() != plainHeaderLen + dataSize && !Recover())
            break;

        LogLn("Payload successfully received, data:");
        LogLn("Signature %llX", sig);
        LogLn("Data Size %d", dataSize);

        auto it = _selectables.find(sig);
        if (it == _selectables.end() && !Recover())
            break;

        it->second.Process(plain.data() + plainHeaderLen);
    }

    LogLn("Selector stopped running");
}

Selector::Selector(std::stop_token st) : _worker(&Selector::Run, this, st)
{
}

} // namespace wsac::run
