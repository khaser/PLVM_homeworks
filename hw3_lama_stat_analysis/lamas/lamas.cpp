#define _GLIBCXX_DEBUG
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
const size_t rare_thresh = 15;

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

static inline code take_code_subseq(int start, int n) {
  int end = start;
  for (int i = 0; i < n; ++i) {
    end += bytecode_data[end].get_size();
  }
  return {bf->to_ip(start), bf->to_ip(end)};
}

template<int N>
struct CodeHash {
  static const size_t A = 4;
  static const size_t B = 543;
  unsigned short operator () (int a) const {
    unsigned short res = B;
    for (auto i : take_code_subseq(a, N)) {
      res = res * A + i; // overflow by powers of two here
    }
    return res;
  }
};

template<int N>
void build_hist(
    std::vector<int> &&starts,
    const std::vector<unsigned short> &hashes,
    std::back_insert_iterator<std::vector<std::pair<int, int>>> inserter,
    std::vector<char> &rare_codes
) {
  auto truthful_cmp = [] (int a, int b) {
    code ac = take_code_subseq(a, N);
    code bc = take_code_subseq(b, N);
    return std::lexicographical_compare(ac.begin(), ac.end(), bc.begin(), bc.end());
  };

  auto is_not_equal = [&truthful_cmp] (int a, int b) {
    code ac = take_code_subseq(a, N);
    code bc = take_code_subseq(b, N);
    return !std::equal(ac.begin(), ac.end(), bc.begin(), bc.end());
  };

  // sort by hash
  std::sort(starts.begin(), starts.end(), [&hashes] (int a, int b) { return hashes[a] < hashes[b]; });

  while (!starts.empty()) {
    int bucket_sz = std::adjacent_find(starts.rbegin(), starts.rend(), [&hashes] (int a, int b) {
      return hashes[a] != hashes[b];
    }) - starts.rbegin() + 1;
    bucket_sz = std::min(bucket_sz, (int) starts.size());

    std::sort(starts.rbegin(), starts.rbegin() + bucket_sz, truthful_cmp);

    while (bucket_sz > 0) {
      int count = std::adjacent_find(starts.rbegin(), starts.rend(), is_not_equal) - starts.rbegin() + 1;
      count = std::min(count, bucket_sz);
      bucket_sz -= count;

      if (count <= rare_thresh) {
        rare_codes[starts.back()] |= (count & 0xf) << (N == 2 ? 4 : 0);
      } else {
        inserter = std::make_pair((N == 2 ? -1 : 1) * starts.back(), count);
      }
      starts.resize(starts.size() - count);
    }
  }
};

} // anon namespace

int main(int argc, char* argv[]) {
  bf = read_file(argv[1]);

  bytecode_data.resize(bf->code_size + 1);
  {
    std::vector<ip_t> ips_to_process;

    auto mark_reachable = [&](ip_t ip) {
      if (!bytecode_data[bf->to_offset(ip)].is_reachable()) {
        int opcode_sz = dispatch<TakeBytecode, ip_t>(ip) - ip;
        bytecode_data[bf->to_offset(ip)].set_size(opcode_sz);
        ips_to_process.push_back(ip);
      }
    };

    auto mark_cf_entry = [&](ip_t ip) {
      mark_reachable(ip);
      bytecode_data[bf->to_offset(ip)].mark_cf_entry();
    };

    bf->call_from_public_ptrs(mark_cf_entry);

    // Traverse to create jump labels & mark reachable code
    while (!ips_to_process.empty()) {
        ip_t ip = ips_to_process.back();
        ips_to_process.pop_back();
        for (auto cont_ip : dispatch<MarkLabels, std::vector<ip_t>>(ip)) {
          mark_reachable(cont_ip);
        }
    }
  }

  // Value:
  // > 0 for single code idiom
  // < 0 for idiom-pairs
  std::vector<char> rare_codes(bf->code_size, 0);
  std::vector<std::pair<int, int>> hist;

  {
    CodeHash<1> hasher;
    std::vector<int> starts;
    std::vector<unsigned short> hashes(bf->code_size);

    // Traverse to calculate idiom occurrences
    for (ip_t ip = bf->code_ptr; ip < bf->code_ptr + bf->code_size; ) {
        if (!bytecode_data[bf->to_offset(ip)].is_reachable()) {
          ip++;
          continue;
        }
        int offset = bf->to_offset(ip);
        starts.emplace_back(offset);
        hashes[offset] = hasher(offset);
        ip += bytecode_data[bf->to_offset(ip)].get_size();
    }

    build_hist<1>(std::move(starts), hashes, std::back_inserter(hist), rare_codes);
  }

  {
    CodeHash<2> hasher;
    std::vector<int> pair_starts;
    std::vector<unsigned short> hashes(bf->code_size);
    std::optional<ip_t> prev_ip;

    // Traverse to calculate idiom-pairs occurrences
    for (ip_t ip = bf->code_ptr; ip < bf->code_ptr + bf->code_size; ) {
        if (!bytecode_data[bf->to_offset(ip)].is_reachable()) {
          prev_ip = {};
          ip++;
          continue;
        }
        if (bytecode_data[bf->to_offset(ip)].is_cf_entry()) {
          prev_ip = {};
        }
        if (prev_ip) {
          int prev_ip_offset = bf->to_offset(*prev_ip);
          pair_starts.emplace_back(prev_ip_offset);
          hashes[prev_ip_offset] = hasher(prev_ip_offset);
        }
        prev_ip = ip;
        ip += bytecode_data[bf->to_offset(ip)].get_size();
    }

    build_hist<2>(std::move(pair_starts), hashes, std::back_inserter(hist), rare_codes);
  }

  std::sort(hist.begin(), hist.end(), [] (const auto &a, const auto &b) { return a.second > b.second; });

  auto print_opcode = [] (int offset, int v) {
    ip_t ip = bf->to_ip(offset);
    if (v < 0) {
      dispatch<PrintCode, void>(ip);
      std::cout << "+ ";
      dispatch<PrintCode, void>(ip + bytecode_data[offset].get_size());
      std::cerr << ": " << -v << '\n';
    } else {
      dispatch<PrintCode, void>(ip);
      std::cout << ": " << v << '\n';
    }
  };

  for (auto [k, v] : hist) {
    print_opcode(k, v);
  }

  for (int v = rare_thresh; v > 0; --v) {
    for (int offset = 0; offset < bf->code_size; ++offset) {
      char x = rare_codes[offset];
      if (x & 0xf0) {
        char xx = ((x >> 4) & 0x0f);
        if (xx == v) print_opcode(offset, -xx);
      }
      if (x & 0x0f) {
        char xx = x & 0x0f;
        if (xx == v) print_opcode(offset, xx);
      }
    }
  }

  return 0;
}
