//
// Created by cve on 2025-07-02.
//

#include "FrameReader.hpp"

#include <Log.hpp>

namespace wsac::io
{

FrameReader::FrameReader(PipeReader &inner) : _inner(inner)
{
}

void FrameReader::ReadUntilPreamble(const std::stop_token &st) const
{
    constexpr auto kPreamble = PREAMBLE;
    constexpr size_t kPreambleSize = kPreamble.size();

    constexpr std::array<size_t, kPreambleSize> kLps = [] {
        std::array<size_t, kPreambleSize> lps{};
        size_t len = 0;

        for (size_t i = 1; i < kPreambleSize; ++i)
        {
            while (len && kPreamble[i] != kPreamble[len])
                len = lps[len - 1];

            if (kPreamble[i] == kPreamble[len])
                ++len;

            lps[i] = len;
        }
        return lps;
    }();

    size_t matched = 0;
    uint8_t b;

    LogVbLn("reading preamble...");
    while (matched < kPreambleSize)
    {
        _inner.Read(b, st);
        while (matched && b != kPreamble[matched])
            matched = kLps[matched - 1];
        if (b == kPreamble[matched])
            ++matched;
    }
    LogVbLn("done reading preamble");
}

model::FrameHeader FrameReader::ReadHeader(const std::stop_token &st) const
{
    auto header = model::FrameHeader::Null();
    _inner.Read(header, st);
    return header;
}

} // namespace wsac::io