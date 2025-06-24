#pragma once

#include "Include.hpp"

#ifdef _DEBUG

#define Log(Format, ...) fprintf(stdout, Format, __VA_ARGS__)
#define LogLn(Format, ...) fprintf(stdout, Format "\n", __VA_ARGS__)

#else

#define Log(Format, ...)
#define LogLn(Format, ...)

#endif