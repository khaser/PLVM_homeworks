#ifndef MARK_LABELS_H
#define MARK_LABELS_H

#include "sm_encoding.h"
#include "bytefile.h"

#include <unordered_map>

struct BytecodeInfo {
  bool jump_label;
  bool reachable;
};

extern std::unordered_map<ip_t, BytecodeInfo> bytecode_data;

template<Bytecodes B, class... Opnds>
struct MarkLabels {
  MarkLabels (const bytefile &bf) {};
  void operator () (code code, const char* str, Opnds... opnds) const {}
};

template<Bytecodes B>
requires (B == BC_CJMPZ || B == BC_JMP || B == BC_CJMPNZ)
struct MarkLabels<B, size_t> {
  const ip_t code_ptr;
  MarkLabels (const bytefile &bf) : code_ptr(bf.code_ptr) {};
  void operator () (code code, const char* str, size_t dest) const {
    bytecode_data[code_ptr + dest].jump_label = true;
  }
};

#endif
