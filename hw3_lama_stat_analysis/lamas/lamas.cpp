#include <memory>
#include <vector>
#include <span>
#include <functional>

#include "bytefile.h"
#include "sm_encoding.h"
#include "mark_labels.h"

std::unordered_map<ip_t, BytecodeInfo> bytecode_data;

template<Bytecodes B, class... Opnds>
struct PrintCode {
  PrintCode (const bytefile &bf) {};
  void operator () (code code, const char* str, Opnds... opnds) const {
    std::cout << str << ' ';
    ((std::cout << std::forward<Opnds>(opnds) << " "), ...);
    std::cout << '\n';
  }
};

template<template<Bytecodes B, class... Opnds> class Func>
inline std::vector<ip_t> dispatch(bytefile& bf, ip_t ip) {

#define GENERIC_IP_PEEK(type) (bf.assert_ip(ip, sizeof(type)), ip += sizeof(type), *(type*)(ip - sizeof(type)))
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
    (Func<BC_JMP, size_t> (bf))({ip - 4, ip}, "JMP", offset);
    ip_t new_ip = bf.code_ptr + offset;
    return {new_ip};
  }

  case BC_CJMPZ: {
    size_t offset = UINT();
    (Func<BC_CJMPZ, size_t> (bf))({ip - 4, ip}, "CJMPZ", offset);
    ip_t new_ip = bf.code_ptr + offset;
    return {ip, new_ip};
  }

  case BC_CJMPNZ: {
    size_t offset = UINT();
    (Func<BC_CJMPNZ, size_t> (bf))({ip - 4, ip}, "CJMPNZ", offset);
    ip_t new_ip = bf.code_ptr + offset;
    return {ip, new_ip};
  }

  case BC_BEGIN: {
    int args = INT();
    int locals = INT();
    (Func<BC_BEGIN, size_t, size_t> (bf))({ip - 8, ip}, "BEGIN", args, locals);
    return {ip};
  }

  case BC_CONST: {
    int imm = INT();
    (Func<BC_CONST, int> (bf))({ip - 4, ip}, "CONST", imm);
    return {ip};
  }

  // case BC_STRING: {
  //   int sz = UINT();
  //   break;
  // }
  //
  // case BC_SEXP: {
  //   const char* tag = get_string(bf, UINT());
  //   size_t sz = UINT();
  //   break;
  // }
  //
  // case BC_STI: {
  //   break;
  // }
  //
  // case BC_STA: {
  //   break;
  // }

  case BC_END: {
    (Func<BC_END> (bf))({ip, ip}, "END");
    return {};
  }

  // case BC_RET: {
  //   break;
  // }

  case BC_DROP: {
    (Func<BC_DROP> (bf))({ip, ip}, "DROP");
    return {ip};
  }

  // case BC_DUP:
  //   break;
  //
  // case BC_SWAP:
  //   break;
  //
  // case BC_ELEM: {
  //   break;
  // }
  //
  // case BC_CBEGIN: {
  //   int args = INT();
  //   int locals = INT();
  //   break;
  // }
  //
  // case BC_CLOSURE: {
  //   int* dest = REF();
  //   int args = INT();
  //   for (int i = 0; i < args; ++i) {
  //     char loc_t = BYTE();
  //     int arg = INT();
  //   }
  //   break;
  // }
  //
  // case BC_CALLC: {
  //   size_t args = UINT();
  //   break;
  // }

  case BC_CALL: {
    size_t args = UINT();
    size_t dest = UINT();
    (Func<BC_CALL, size_t, size_t> (bf))({ip - 8, ip}, "CALL", args, dest);
    ip_t callee_ip = bf.code_ptr + dest;
    return {ip, callee_ip};
  }

  // case BC_TAG: {
  //   const char* tag = get_string(bf, UINT());
  //   int args = INT();
  //   break;
  // }
  //
  // case BC_ARRAY: {
  //   int sz = INT();
  //   break;
  // }
  //
  // case BC_FAIL: {
  //   int line = INT();
  //   int col = INT();
  //   break;
  // }

  case BC_LINE: {
    int line = INT();
    (Func<BC_LINE, int> (bf))({ip - 4, ip}, "LINE", line);
    return {ip};
  }

  case BC_LREAD: {
    (Func<BC_LREAD> (bf))({ip, ip}, "CALL Lread");
    return {ip};
  }

  case BC_LWRITE: {
    (Func<BC_LWRITE> (bf))({ip, ip}, "CALL Lwrite");
    return {ip};
  }

  // case BC_LLENGTH:
  //   break;
  //
  // case BC_LSTRING:
  //   break;
  //
  // case BC_BARRAY: {
  //   size_t len = UINT();
  //   break;
  // }
  //
  // case BC_PATT_STR:
  //   break;


    default: {
      switch (h) {

        case BC_LD: {
          int idx = INT();
          (Func<BC_LD, int> (bf))({ip - 4, ip}, "LD", idx);
          return {ip};
        }

        case BC_ST: {
          int idx = INT();
          (Func<BC_ST, int> (bf))({ip - 4, ip}, "ST", idx);
          return {ip};
        }

        // case BC_STOP: {
        //   goto stop;
        // }

        case BC_BINOP: {
          (Func<BC_BINOP> (bf))({ip - 4, ip}, "BINOP");
          return {ip};
        }

        // case BC_LDA: {
        //   int idx = INT();
        //   break;
        // }
        //
        // case BC_PATT: {
        //   break;
        // }

        default: {
          std::cerr << "Invalid opcode " << h << "-" << l << "\n";
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

int main(int argc, char* argv[]) {
  bytefile bf = read_file(argv[1]);

  std::vector<ip_t> ips_to_process = bf.get_public_ptrs();
  // std::unordered_map<code, int> idiom_stat;

  // Traverse to create jump labels & mark reachable code
  while (!ips_to_process.empty()) {
      ip_t ip = ips_to_process.back();
      ips_to_process.pop_back();
      if (bytecode_data[ip].reachable) continue;
      bytecode_data[ip].reachable = true;
      // std::cerr << "TRAVERSE 1: " << (void*) ip << ' ' << (int)*(unsigned char*)ip << '\n';
      dispatch<PrintCode>(bf, ip);
      for (auto cont_ip : dispatch<MarkLabels>(bf, ip)) {
        ips_to_process.push_back(cont_ip);
      }
  }

  // for (auto i = bf.code_ptr; i != bf.code_ptr + bf.code_size; ++i) {
  //   BytecodeInfo k = bytecode_data[i];
  //   std::cout << (void*) i << ' ' << k.reachable << ' ' << k.jump_label << '\n';
  // }


  return 0;
}
