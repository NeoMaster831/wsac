#pragma once

#include "Include.hpp"

namespace wsac::run::comm
{
#pragma pack(push)
#pragma pack(1)
struct ReportHeader
{
    uint64_t Signature = PAYLOAD_SIGNATURE;
    uint64_t RequestId;
    uint64_t Id;
    int32_t Status;
    uint64_t Timestamp;

    ReportHeader(uint64_t requestId, uint64_t id, int32_t status);

private:
    friend class ReportBuilder;

    ReportHeader() : Signature(0), RequestId(0), Id(0), Status(0), Timestamp(0) {}

    bool operator!() const noexcept
    {
        return Signature != PAYLOAD_SIGNATURE;
    }
};
#pragma pack(pop)

class ReportBuilder
{
    ReportHeader _header;
    std::vector<uint8_t> *_body;

  public:
    ReportBuilder() : _body(nullptr) {}

    ReportBuilder &Header(const ReportHeader &header)
    {
        _header = header;
        return *this;
    }

    ReportBuilder &Body(std::vector<uint8_t> &body)
    {
        if (!body.empty())
            _body = &body;
        return *this;
    }

    bool Send() const noexcept;
};

bool Initialize() noexcept;

void Terminate() noexcept;

} // namespace wsac::run::comm