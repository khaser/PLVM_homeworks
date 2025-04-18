#ifndef UTIL_H
#define UTIL_H

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

#include <iostream>
#include <vector>

#include "sm_encoding.h"

using ip_t = const unsigned char*;

struct BcFunction {
  int begin, end;
  int args;
  int locals;
  int captured;
  int min_rel_st_size;
  int max_rel_st_size;
};

struct BcData {
  bool reachable;
  int rel_st_size;
};

struct BcLoc {
  unsigned char type;
  int idx;
};

std::ostream& operator << (std::ostream &stream, const BcLoc &loc);
std::ostream& operator << (std::ostream &stream, const std::vector<BcLoc> &locs);

namespace {

template<Bytecodes B, class... Opnds>
struct PrintCode {
  void operator () (int offset, int sz, const char* str, Opnds... opnds) const {
    std::cout << str << ' ';
    ((std::cout << std::forward<Opnds>(opnds) << " "), ...);
  }
};

} // anon namespace

#endif
