#include <utility>

#include "Run/Detector.hpp"

std::vector<wsac::run::Detector *> &GetLocalDetectors()
{
    static std::vector<wsac::run::Detector *> detectors;
    return detectors;
}

static size_t currentId = 0;

wsac::run::Detector::Detector(const std::function<bool()> &check, std::string desc)
    : Id(currentId++), Description(std::move(desc)), Check(check)
{
    GetLocalDetectors().push_back(this);
}

std::vector<wsac::run::Detector *> &wsac::run::Detector::GetDetectors()
{
    return GetLocalDetectors();
}
