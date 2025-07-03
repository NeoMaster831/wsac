#pragma once

#include "Include.hpp"

#define LOG_VERBOSE

#ifdef _DEBUG

#define Log(Format, ...) fprintf(stdout, Format, __VA_ARGS__)
#define LogLn(Format, ...) fprintf(stdout, Format "\n", __VA_ARGS__)
#ifdef LOG_VERBOSE
#define LogVb(Format, ...) Log(Format, __VA_ARGS__)
#define LogVbLn(Format, ...) LogLn(Format, __VA_ARGS__)
#else
#define LogVb(Format, ...)
#define LogVbLn(Format, ...)
#endif

#else

#define Log(Format, ...)
#define LogLn(Format, ...)
#define LogVb(Format, ...)
#define LogVbLn(Format, ...)

#endif