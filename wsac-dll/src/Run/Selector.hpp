#pragma once

#include "Include.hpp"

namespace wsac::run
{

using Signature = uint64_t;

interface ISelectable
{
    void Process(void* buffer);
};

class Selector
{
    std::unordered_map<Signature, ISelectable> _selectables;
    std::jthread _worker;

    void Run(const std::stop_token &st);

  public:
    explicit Selector(std::stop_token st);
};

} // namespace wsac::run
