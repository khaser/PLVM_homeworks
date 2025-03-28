#ifndef MARK_LABELS_H
#define MARK_LABELS_H

#include "sm_encoding.h"
#include "bytefile.h"

#include <unordered_map>

struct BytecodeInfo {
private:
  int size;
  static const int cf_entry_mask = 1 << (sizeof(int) * 8 - 1);
public:
  inline void mark_cf_entry() {
    size = cf_entry_mask | size;
  }

  inline bool is_reachable() {
    return get_size();
  }

  inline bool is_cf_entry() {
    return (size & cf_entry_mask);
  }

  inline void set_size(int x) {
    size = (size & cf_entry_mask) | (x & (~cf_entry_mask));
  }

  inline int get_size() {
    return size & (~cf_entry_mask);
  }
};

extern std::vector<BytecodeInfo> bytecode_data;

namespace {

template<Bytecodes B, class... Opnds>
struct MarkLabels {
  const ip_t code_ptr;
  MarkLabels (const bytefile *bf) : code_ptr(bf->code_ptr) {};
  std::vector<ip_t> operator () (code code, const char* str, Opnds... opnds) const {
    ip_t c = code.data() + code.size();
    if (B == BC_END) {
      bytecode_data[c - code_ptr].mark_cf_entry();
    }
    if (B == BC_END || B == BC_STOP) return {};
    else return {c};
  }
};

template<Bytecodes B>
requires (B == BC_CJMPZ || B == BC_JMP || B == BC_CJMPNZ)
struct MarkLabels<B, size_t> {
  const ip_t code_ptr;
  MarkLabels (const bytefile *bf) : code_ptr(bf->code_ptr) {};
  std::vector<ip_t> operator () (code code, const char* str, size_t offset) const {
    ip_t new_ip = code_ptr + offset;
    bytecode_data[offset].mark_cf_entry();
    if (B == BC_JMP) {
      return {new_ip};
    } else {
      return {code.data() + code.size(), new_ip};
    }
  }
};

template<Bytecodes B>
requires (B == BC_CALL || B == BC_CLOSURE)
struct MarkLabels<B, size_t, size_t> {
  const ip_t code_ptr;
  MarkLabels (const bytefile *bf) : code_ptr(bf->code_ptr) {};
  std::vector<ip_t> operator () (code code, const char* str, size_t dest, size_t args) const {
    ip_t new_ip = code_ptr + dest;
    bytecode_data[dest].mark_cf_entry();
    return {code.data() + code.size(), new_ip};
  }
};

} // anon namespace

#endif
