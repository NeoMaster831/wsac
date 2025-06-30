#pragma once

#include "Include.hpp"

namespace wsac::sec
{

class Cryptography;

class CryptoService
{
  public:
    CryptoService();

    static void InstallServerKey(const std::vector<uint8_t> &key);
};

} // namespace wsac::sec
