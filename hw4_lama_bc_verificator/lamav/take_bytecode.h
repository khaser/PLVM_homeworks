#ifndef TAKE_BYTECODE_H
#define TAKE_BYTECODE_H

#include "sm_encoding.h"
#include "util.h"
#include <iostream>

namespace {

template<Bytecodes B, class... Opnds>
struct PrintCode {
  void operator () (int offset, int sz, const char* str, Opnds... opnds) const {
    std::cout << str << ' ';
    ((std::cout << std::forward<Opnds>(opnds) << " "), ...);
  }
};

// template<Bytecodes B, class... Opnds>
// struct TakeBytecode {
//   ip_t operator () (code code, const char* str, Opnds... opnds) const {
//     return code.data() + code.size();
//   }
// };

} // anon namespace

#endif
