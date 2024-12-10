#ifndef UTIL_H
#define UTIL_H

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

using ip_t = const unsigned char*;

#include <span>

using code = std::span<const unsigned char>;

struct CodeHash {
  static const size_t A = 4;
  static const size_t B = 543;
  size_t operator () (const code &a) const {
    size_t res = 1;
    for (auto i : a) {
      res = i * A + B; // overflow by powers of two here
    }
    return res;
  }
};

struct CodeComparator {
  bool operator () (const code &a, const code &b) const {
    return std::equal(a.begin(), a.end(), b.begin(), b.end());
  }
};

#endif
