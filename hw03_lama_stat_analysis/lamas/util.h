#ifndef UTIL_H
#define UTIL_H

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

using ip_t = const unsigned char*;

#include <span>

using code = std::span<const unsigned char>;

#endif
