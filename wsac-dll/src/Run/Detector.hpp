#pragma once

#include "Include.hpp"

#define DECL_DETECTOR(fn, desc)                                                                                        \
    static wsac::run::Detector fn##Detector                                                                            \
    {                                                                                                                  \
        (fn), (desc)                                                                                                   \
    }

namespace wsac::run
{

struct Detector
{
    size_t Id;
    std::string Description;
    std::function<bool()> Check;

    Detector(const std::function<bool()> &check, std::string desc);

    static std::vector<Detector *> &GetDetectors();
};

} // namespace wsac::run