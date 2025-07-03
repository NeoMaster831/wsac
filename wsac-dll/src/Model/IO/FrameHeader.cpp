//
// Created by cve on 2025-07-02.
//

#include "FrameHeader.hpp"

#include "Host.hpp"
#include "Model/Bytes.hpp"
#include "Run/State.hpp"
#include "Utils/Crypto/Random.hpp"

namespace wsac::model
{

FrameHeader::FrameHeader(const FrameSig sig, const uint64_t size) : sig(sig), dataSize(size)
{
    Sign();
}

bool FrameHeader::Validate() const
{
    FrameHeader copy = *this;
    copy.Sign();
    return mac == copy.mac;
}

void FrameHeader::Sign()
{
    crypto::Random::Fill(nonce);

    const Bytes ref{reinterpret_cast<uint8_t *>(this), sizeof sig + sizeof dataSize};

    const auto &cipher = Host::Current().Get<run::State>().Layer1PskEncryptor;
    cipher.Sign(ref, nonce, mac);
}

FrameHeader FrameHeader::Null()
{
    static FrameHeader header{FrameSig::Null, 0};
    return header;
}

} // namespace wsac::model