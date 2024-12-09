#include <unordered_map>
#include <memory>
#include <vector>
#include <span>
#include <functional>
#include <optional>

#include "bytefile.h"
#include "sm_encoding.h"
#include "mark_labels.h"
#include "take_bytecode.h"

bytefile *bf;
std::vector<BytecodeInfo> bytecode_data;

namespace {

template<template<Bytecodes B, class... Opnds> class Func, class T>
inline T dispatch(ip_t ip) {

#define GENERIC_IP_PEEK(type) (bf->assert_ip(ip, sizeof(type)), ip += sizeof(type), *(type*)(ip - sizeof(type)))
#define INT()    (GENERIC_IP_PEEK(int))
#define UINT()   (GENERIC_IP_PEEK(size_t))
#define REF()    (GENERIC_IP_PEEK(int*))
#define BYTE()   (GENERIC_IP_PEEK(unsigned char))

  unsigned char x = BYTE(),
       h = (x & 0xF0) >> 4,
       l = x & 0x0F;

  switch (x) {

  case BC_JMP: {
    size_t offset = UINT();
    return (Func<BC_JMP, size_t> (bf))({ip - 5, ip}, "JMP", offset);
  }

  case BC_CJMPZ: {
    size_t offset = UINT();
    return (Func<BC_CJMPZ, size_t> (bf))({ip - 5, ip}, "CJMPZ", offset);
  }

  case BC_CJMPNZ: {
    size_t offset = UINT();
    return (Func<BC_CJMPNZ, size_t> (bf))({ip - 5, ip}, "CJMPNZ", offset);
  }

  case BC_BEGIN: {
    int args = INT();
    int locals = INT();
    return (Func<BC_BEGIN, size_t, size_t> (bf))({ip - 9, ip}, "BEGIN", args, locals);
  }

  case BC_CONST: {
    int imm = INT();
    return (Func<BC_CONST, int> (bf))({ip - 5, ip}, "CONST", imm);
  }

  case BC_STRING: {
    int sz = UINT();
    return (Func<BC_STRING, int> (bf))({ip - 5, ip}, "STRING", sz);
  }

  case BC_SEXP: {
    size_t tag_idx = UINT();
    size_t sz = UINT();
    return (Func<BC_SEXP, size_t, size_t> (bf))({ip - 9, ip}, "S_EXP", tag_idx, sz);
  }

  case BC_STI: {
    return (Func<BC_STI> (bf))({ip - 1, ip}, "STI");
  }

  case BC_STA: {
    return (Func<BC_STA> (bf))({ip - 1, ip}, "STA");
  }

  case BC_END: {
    return (Func<BC_END> (bf))({ip - 1, ip}, "END");
  }

  case BC_RET: {
    return (Func<BC_RET> (bf))({ip - 1, ip}, "RET");
  }

  case BC_DROP: {
    return (Func<BC_DROP> (bf))({ip - 1, ip}, "DROP");
  }

  case BC_DUP: {
    return (Func<BC_DUP> (bf))({ip - 1, ip}, "DUP");
  }

  case BC_SWAP: {
    return (Func<BC_SWAP> (bf))({ip - 1, ip}, "SWAP");
  }

  case BC_ELEM: {
    return (Func<BC_ELEM> (bf))({ip - 1, ip}, "ELEM");
  }

  case BC_CBEGIN: {
    int args = INT();
    int locals = INT();
    return (Func<BC_CBEGIN, size_t, size_t> (bf))({ip - 9, ip}, "CBEGIN", args, locals);
  }

  case BC_CLOSURE: {
    size_t dest = UINT();
    size_t args = UINT();
    size_t bc_sz = 9;
    // std::cerr << "HERE " << (void*) dest << ' ' << args << '\n';
    for (int i = 0; i < args; ++i) {
      char loc_t = BYTE();
      int arg = INT();
      bc_sz += 5;
    }
    return (Func<BC_CLOSURE, size_t, size_t> (bf))({ip - bc_sz, ip}, "CLOSURE", dest, args);
  }

  case BC_CALLC: {
    size_t args = UINT();
    return (Func<BC_CALLC, size_t> (bf))({ip - 5, ip}, "CALLC", args);
  }

  case BC_CALL: {
    size_t dest = UINT();
    size_t args = UINT();
    return (Func<BC_CALL, size_t, size_t> (bf))({ip - 9, ip}, "CALL", dest, args);
  }

  case BC_TAG: {
    size_t tag_idx = UINT();
    size_t args = UINT();
    return (Func<BC_TAG, size_t, size_t> (bf))({ip - 9, ip}, "TAG", tag_idx, args);
  }

  case BC_ARRAY: {
    size_t sz = UINT();
    return (Func<BC_ARRAY, size_t> (bf))({ip - 5, ip}, "ARRAY", sz);
  }

  case BC_FAIL: {
    size_t line = UINT();
    size_t col = UINT();
    return (Func<BC_FAIL, size_t, size_t> (bf))({ip - 9, ip}, "FAIL", line, col);
  }

  case BC_LINE: {
    size_t line = INT();
    return (Func<BC_LINE, size_t> (bf))({ip - 5, ip}, "LINE", line);
  }

  case BC_LREAD: {
    return (Func<BC_LREAD> (bf))({ip, ip}, "CALL Lread");
  }

  case BC_LWRITE: {
    return (Func<BC_LWRITE> (bf))({ip, ip}, "CALL Lwrite");
  }

  case BC_LLENGTH: {
    return (Func<BC_LLENGTH> (bf))({ip - 1, ip}, "LLENGTH");
  }

  case BC_LSTRING: {
    return (Func<BC_LSTRING> (bf))({ip - 1, ip}, "LSTRING");
  }

  case BC_BARRAY: {
    size_t sz = UINT();
    return (Func<BC_BARRAY, size_t> (bf))({ip - 4, ip}, "BARRAY", sz);
  }

  case BC_PATT_STR: {
    return (Func<BC_PATT_STR> (bf))({ip - 1, ip}, "PATT_STR");
  }

    default: {
      switch (h) {

        case BC_LD: {
          int idx = INT();
          return (Func<BC_LD, int> (bf))({ip - 5, ip}, "LD", idx);
        }

        case BC_ST: {
          int idx = INT();
          return (Func<BC_ST, int> (bf))({ip - 5, ip}, "ST", idx);
        }

        case BC_STOP: {
          return (Func<BC_STOP> (bf))({ip - 1, ip}, "STOP");
        }

        case BC_BINOP: {
          return (Func<BC_BINOP> (bf))({ip - 5, ip}, "BINOP");
        }

        case BC_LDA: {
          size_t idx = INT();
          return (Func<BC_LDA> (bf))({ip - 5, ip}, "LDA");
        }

        case BC_PATT: {
          return (Func<BC_PATT> (bf))({ip - 1, ip}, "PATT");
        }

        default: {
          std::cerr << "Invalid opcode " << (int) h << "-" << (int) l << "\n";
          exit(1);
        }
      }
   }
  }
#undef GENERIC_IP_PEEK
#undef INT
#undef UINT
#undef REF
#undef BYTE
}

inline code take_code_subseq(int offset, int n) {
  ip_t start = bf->to_ip(offset);
  ip_t end = start;
  for (int i = 0; i < n; ++i) {
    end = dispatch<TakeBytecode, ip_t>(end);
  }
  return {start, end};
}

template<int N>
struct CodeHash {
  static const size_t A = 4;
  static const size_t B = 543;
  size_t operator () (int a) const {
    size_t res = 1;
    for (auto i : take_code_subseq(a, N)) {
      res = i * A + B; // overflow by powers of two here
    }
    return res;
  }
};

template<int N>
struct CodeComparator {
  bool operator () (int a, int b) const {
    code ac = take_code_subseq(a, N);
    code bc = take_code_subseq(b, N);
    return std::equal(ac.begin(), ac.end(), bc.begin(), bc.end());
  }
};

} // anon namespace

int main(int argc, char* argv[]) {
  bf = read_file(argv[1]);

  std::vector<ip_t> ips_to_process = bf->get_public_ptrs();
  bytecode_data.resize(bf->code_size);

  for (auto ip : ips_to_process) {
    bytecode_data[bf->to_offset(ip)].jump_label = true;
  }

  // Traverse to create jump labels & mark reachable code
  while (!ips_to_process.empty()) {
      ip_t ip = ips_to_process.back();
      ips_to_process.pop_back();
      for (auto cont_ip : dispatch<MarkLabels, std::vector<ip_t>>(ip)) {
        if (!bytecode_data[bf->to_offset(cont_ip)].reachable) {
          ips_to_process.push_back(cont_ip);
          bytecode_data[bf->to_offset(cont_ip)].reachable = true;
        }
      }
  }

  std::optional<ip_t> prev_ip;
  std::unordered_map<int, int, CodeHash<1>, CodeComparator<1>> stats;
  std::unordered_map<int, int, CodeHash<2>, CodeComparator<2>> pair_stats;

  // Traverse to calculate idioms occurrences
  for (ip_t ip = bf->code_ptr; ip < bf->code_ptr + bf->code_size; ) {
      if (!bytecode_data[bf->to_offset(ip)].reachable) {
        prev_ip = {};
        ip++;
        continue;
      }

      ip_t next_ip = dispatch<TakeBytecode, ip_t>(ip);
      // std::cout << (void*) (ip - bf->code_ptr) << ' ';
      // dispatch<PrintCode, void>(bf, ip);
      // std::cout << '\n';
      if (bytecode_data[bf->to_offset(ip)].jump_label) {
        prev_ip = {};
      }

      stats[bf->to_offset(ip)]++;
      if (prev_ip) {
        pair_stats[bf->to_offset(*prev_ip)]++;
      }
      prev_ip = ip;
      ip = next_ip;
  }
  std::cout << '\n';

  std::vector<std::pair<int, int>> v_stats(stats.begin(), stats.end());
  std::vector<std::pair<int, int>> v_pair_stats(pair_stats.begin(), pair_stats.end());
  std::sort(v_stats.begin(), v_stats.end(), [] (const auto &a, const auto &b) { return a.second > b.second; });
  std::sort(v_pair_stats.begin(), v_pair_stats.end(), [] (const auto &a, const auto &b) { return a.second > b.second; });

  std::cout << "Single idioms occurrences:\n";
  for (auto [k, v] : v_stats) {
    dispatch<PrintCode, void>(bf->to_ip(k));
    std::cout << ": " << v << '\n';
  }
  std::cout << "\nIdiom-pairs occurrences:\n";
  for (auto [k, v] : v_pair_stats) {
    dispatch<PrintCode, void>(bf->to_ip(k));
    std::cout << "+ ";
    dispatch<PrintCode, void>(dispatch<TakeBytecode, ip_t>(bf->to_ip(k)));
    std::cerr << ": " << v << '\n';
  }

  return 0;
}
