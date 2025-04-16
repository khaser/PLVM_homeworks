#include <vector>

#include "bytefile.h"
#include "sm_encoding.h"

namespace {

template<
  template<Bytecodes B, class... Opnds> class Func,
  class T,
  class... Opnds
>
inline T dispatch(int bc_offset, bytefile* bf, Opnds&& ...opnds) {

#define GENERIC_IP_PEEK(type) (ip += sizeof(type), *(type*)(ip - sizeof(type)))
#define INT()    (GENERIC_IP_PEEK(int))
#define REF()    (GENERIC_IP_PEEK(int*))
#define BYTE()   (GENERIC_IP_PEEK(unsigned char))
  ip_t ip = bf->to_ip(bc_offset);

  unsigned char x = BYTE(),
       h = (x & 0xF0) >> 4,
       l = x & 0x0F;

  switch (x) {

  case BC_JMP: {
    int offset = INT();
    return Func<BC_JMP, int>()(bc_offset, 5, "JMP", std::forward<Opnds>(opnds)..., offset);
  }

  case BC_CJMPZ: {
    int offset = INT();
    return Func<BC_CJMPZ, int>()(bc_offset, 5, "CJMPZ", std::forward<Opnds>(opnds)..., offset);
  }

  case BC_CJMPNZ: {
    int offset = INT();
    return Func<BC_CJMPNZ, int>()(bc_offset, 5, "CJMPNZ", std::forward<Opnds>(opnds)..., offset);
  }

  case BC_BEGIN: {
    int args = INT();
    int locals = INT();
    return Func<BC_BEGIN, int, int>()(bc_offset, 9, "BEGIN", std::forward<Opnds>(opnds)..., args, locals);
  }

  case BC_CONST: {
    int imm = INT();
    return Func<BC_CONST, int>()(bc_offset, 5, "CONST", std::forward<Opnds>(opnds)..., imm);
  }

  case BC_STRING: {
    int sz = INT();
    return Func<BC_STRING, int>()(bc_offset, 5, "STRING", std::forward<Opnds>(opnds)..., sz);
  }

  case BC_SEXP: {
    int tag_idx = INT();
    int sz = INT();
    return Func<BC_SEXP, int, int>()(bc_offset, 9, "S_EXP", std::forward<Opnds>(opnds)..., tag_idx, sz);
  }

  case BC_STI: {
    return Func<BC_STI>()(bc_offset, 1, "STI", std::forward<Opnds>(opnds)...);
  }

  case BC_STA: {
    return Func<BC_STA>()(bc_offset, 1, "STA", std::forward<Opnds>(opnds)...);
  }

  case BC_END: {
    return Func<BC_END>()(bc_offset, 1, "END", std::forward<Opnds>(opnds)...);
  }

  case BC_RET: {
    return Func<BC_RET>()(bc_offset, 1, "RET", std::forward<Opnds>(opnds)...);
  }

  case BC_DROP: {
    return Func<BC_DROP>()(bc_offset, 1, "DROP", std::forward<Opnds>(opnds)...);
  }

  case BC_DUP: {
    return Func<BC_DUP>()(bc_offset, 1, "DUP", std::forward<Opnds>(opnds)...);
  }

  case BC_SWAP: {
    return Func<BC_SWAP>()(bc_offset, 1, "SWAP", std::forward<Opnds>(opnds)...);
  }

  case BC_ELEM: {
    return Func<BC_ELEM>()(bc_offset, 1, "ELEM", std::forward<Opnds>(opnds)...);
  }

  case BC_CBEGIN: {
    int args = INT();
    int locals = INT();
    return Func<BC_CBEGIN, int, int>()(bc_offset, 9, "CBEGIN", std::forward<Opnds>(opnds)..., args, locals);
  }

  case BC_CLOSURE: {
    int dest = INT();
    int args = INT();
    int bc_sz = 9;
    std::vector<BcLoc> capture;
    for (int i = 0; i < args; ++i) {
      char loc_t = BYTE();
      int arg = INT();
      capture.emplace_back(loc_t, arg);
      bc_sz += 5;
    }
    return Func<BC_CLOSURE, int, int, std::vector<BcLoc>>()(bc_offset, bc_sz, "CLOSURE", std::forward<Opnds>(opnds)..., dest, args, capture);
  }

  case BC_CALLC: {
    int args = INT();
    return Func<BC_CALLC, int>()(bc_offset, 5, "CALLC", std::forward<Opnds>(opnds)..., args);
  }

  case BC_CALL: {
    int dest = INT();
    int args = INT();
    return Func<BC_CALL, int, int>()(bc_offset, 9, "CALL", std::forward<Opnds>(opnds)..., dest, args);
  }

  case BC_TAG: {
    int tag_idx = INT();
    int args = INT();
    return Func<BC_TAG, int, int>()(bc_offset, 9, "TAG", std::forward<Opnds>(opnds)..., tag_idx, args);
  }

  case BC_ARRAY: {
    int sz = INT();
    return Func<BC_ARRAY, int>()(bc_offset, 5, "ARRAY", std::forward<Opnds>(opnds)..., sz);
  }

  case BC_FAIL: {
    int line = INT();
    int col = INT();
    return Func<BC_FAIL, int, int>()(bc_offset, 9, "FAIL", std::forward<Opnds>(opnds)..., line, col);
  }

  case BC_LINE: {
    int line = INT();
    return Func<BC_LINE, int>()(bc_offset, 5, "LINE", std::forward<Opnds>(opnds)..., line);
  }

  case BC_LREAD: {
    return Func<BC_LREAD>()(bc_offset, 1, "CALL Lread", std::forward<Opnds>(opnds)...);
  }

  case BC_LWRITE: {
    return Func<BC_LWRITE>()(bc_offset, 1, "CALL Lwrite", std::forward<Opnds>(opnds)...);
  }

  case BC_LLENGTH: {
    return Func<BC_LLENGTH>()(bc_offset, 1, "LLENGTH", std::forward<Opnds>(opnds)...);
  }

  case BC_LSTRING: {
    return Func<BC_LSTRING>()(bc_offset, 1, "LSTRING", std::forward<Opnds>(opnds)...);
  }

  case BC_BARRAY: {
    int sz = INT();
    return Func<BC_BARRAY, int>()(bc_offset, 5, "BARRAY", std::forward<Opnds>(opnds)..., sz);
  }

  case BC_PATT_STR: {
    return Func<BC_PATT_STR>()(bc_offset, 1, "PATT_STR", std::forward<Opnds>(opnds)...);
  }

    default: {
      switch (h) {

        case BC_LD: {
          int idx = INT();
          return Func<BC_LD, BcLoc>()(bc_offset, 5, "LD", std::forward<Opnds>(opnds)..., {l, idx});
        }

        case BC_ST: {
          int idx = INT();
          return Func<BC_ST, BcLoc>()(bc_offset, 5, "ST", std::forward<Opnds>(opnds)..., {l, idx});
        }

        case BC_STOP: {
          return Func<BC_STOP>()(bc_offset, 1, "STOP", std::forward<Opnds>(opnds)...);
        }

        case BC_BINOP: {
          return Func<BC_BINOP>()(bc_offset, 1, "BINOP", std::forward<Opnds>(opnds)...);
        }

        case BC_LDA: {
          int idx = INT();
          return Func<BC_LDA, BcLoc>()(bc_offset, 5, "LDA", std::forward<Opnds>(opnds)..., {l, idx});
        }

        case BC_PATT: {
          return Func<BC_PATT>()(bc_offset, 1, "PATT", std::forward<Opnds>(opnds)...);
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
#undef REF
#undef BYTE
}

} // anon namespace
