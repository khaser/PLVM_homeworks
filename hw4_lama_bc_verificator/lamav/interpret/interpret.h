#ifndef INTERPRET_H
#define INTERPRET_H

#include "../bytefile.h"

#include <iostream>

extern "C" {
#include "../../runtime/gc.h"
#include "../../runtime/runtime.h"
}

#include "../sm_encoding.h"
#include "../bytefile.h"

#include "vstack.h"
#include "runtime_externs.h"

/* Mostly copy-pasted from lamai */
/* TODO: rewrite with `dispatch` */

struct frame {
    size_t*        vstack_base;
    size_t      locals;
    size_t      args;
    char        is_closure;
    const void* ret_ip;
} fstack[MAX_FRAME_STACK_SZ];

struct frame* const main_frame = fstack + MAX_FRAME_STACK_SZ - 1;

size_t vstack[MAX_VSTACK_SZ];

namespace {

inline int binop(char opcode, int a, int b) {
    switch (opcode) {
#define DEF(opcode, op) case opcode: return a op b; break;
    BINOPS(DEF)
#undef DEF
    default:
      failure("Unsupported opcode for binary operation: %d", (int)opcode);
  }
}

inline size_t* resolve_loc(const struct frame* cur_frame, char loc_type, int idx) {
  switch (loc_type) {
    case GLOBAL:
      return __gc_stack_bottom - 1 - idx;
    case LOCAL:
      return cur_frame->vstack_base - idx;
    case ARGUMENT:
      return cur_frame->args + cur_frame->vstack_base - idx;
    case CAPTURED:
      size_t* cls = *(size_t**)(cur_frame->args + cur_frame->vstack_base + 1);
      return cls + 1 + idx;
  }
  failure("Unsupported memory operation operand type: %d", (int)loc_type);
}


} // anon namespace


int interpret(bytefile *bf, char* filename) {

  __init();
  __gc_stack_bottom = vstack + MAX_VSTACK_SZ;
  __gc_stack_top = vstack + MAX_VSTACK_SZ - 1;

  ALLOC(bf->global_area_size);
  struct frame* cur_frame = main_frame;
  cur_frame->vstack_base = __gc_stack_top;

  ip_t ip = bf->code_ptr;

#define GENERIC_IP_PEEK(type) (ip += sizeof(type), *(type*)(ip - sizeof(type)))
#define INT()    (GENERIC_IP_PEEK(int))
#define UINT()   (GENERIC_IP_PEEK(size_t))
#define REF()    (GENERIC_IP_PEEK(int*))
#define BYTE()   (GENERIC_IP_PEEK(unsigned char))
#define FAIL()   failure ("Invalid opcode %d-%d\n", h, l)

#define MAKE_JUMP(offset) (ip = bf->code_ptr + (offset))

  do {
    unsigned char x = BYTE(),
         h = (x & 0xF0) >> 4,
         l = x & 0x0F;

    switch (x) {
    case BC_CONST: {
      int val = INT();
      PUSH(BOX(val));
      break;
    }

    case BC_STRING: {
      PUSH_REF(Bstring(bf->get_string(UINT())));
      break;
    }

    case BC_SEXP: {
      const char* tag = bf->get_string(UINT());
      size_t sz = UINT();
      void* bsexp = Bsexp(sz, UNBOX(LtagHash(tag)));
      PUSH_REF(bsexp);
      break;
    }

    case BC_STI: {
      int val = POP();
      size_t* dest = POP_REF();
      PUSH(*dest = val);
      break;
    }

    case BC_STA: {
      size_t* arr = POP_REF();
      int idx = POP();
      size_t* val_ptr = POP_REF();
      PUSH_REF(Bsta(arr, idx, val_ptr));
      break;
    }

    case BC_JMP: {
      size_t offset = UINT();
      MAKE_JUMP(offset);
      break;
    }

    case BC_END:
    case BC_RET: {
      int ret = POP();
      TRUNC(cur_frame->locals + cur_frame->args + cur_frame->is_closure);
      PUSH(ret);
      ip = static_cast<ip_t>(cur_frame->ret_ip);
      if ((cur_frame++) == main_frame) {
        goto stop;
      }
      break;
    }

    case BC_DROP:
      POP();
      break;

    case BC_DUP:
      PUSH(TOP());
      break;

    case BC_SWAP: {
      int a = POP();
      int b = POP();
      PUSH(b);
      PUSH(a);
      break;
    }

    case BC_ELEM: {
      int idx = POP();
      size_t* arr = POP_REF();
      PUSH_REF(Belem(arr, idx));
      break;
    }

    case BC_CJMPZ:
    case BC_CJMPNZ: {
      size_t offset = UINT();
      if (1 ^ l ^ !!UNBOX(POP())) {
        MAKE_JUMP(offset);
      }
      break;
    }

    case BC_BEGIN:
    case BC_CBEGIN: {
      int args = INT();
      int locals = INT();
      ALLOC(locals);
      cur_frame->locals = locals;
      break;
    }

    case BC_CLOSURE: {
      int* dest = REF();
      int args = INT();
      for (int i = 0; i < args; ++i) {
        char loc_t = BYTE();
        int arg = INT();
        PUSH(*resolve_loc(cur_frame, loc_t, arg));
      }
      void* cls = Bclosure(args, dest);
      PUSH_REF(cls);
      break;
    }

    {
      size_t args, dest;
      size_t* cls;

    case BC_CALLC:
      args = UINT();
      cls = (size_t*) *(__gc_stack_top + 1 + args);
      goto call;

    case BC_CALL:
      dest = UINT();
      args = UINT();
      cls = 0;

    call:
      cur_frame--;
      cur_frame->ret_ip = ip;
      cur_frame->vstack_base = __gc_stack_top;
      cur_frame->args = args;
      cur_frame->is_closure = !!cls;
      /*
      CLOSURE?
      ARG 1
      ARG 2
      LOCAL 1 <- vstack_base
      LOCAL 2
      RET_VAL
      */

      MAKE_JUMP(cls ? *cls : dest);
      break;
    }

    case BC_TAG: {
      const char* tag = bf->get_string(UINT());
      int args = INT();
      size_t* sexp = POP_REF();
      PUSH(Btag(sexp, LtagHash(tag), BOX(args)));
      break;
    }

    case BC_ARRAY: {
      int sz = INT();
      size_t* arr = POP_REF();
      PUSH(Barray_patt(arr, BOX(sz)));
      break;
    }

    case BC_FAIL: {
      int line = INT();
      int col = INT();
      Bmatch_failure(POP_REF(), filename, line, col);
      break;
    }

    case BC_LINE:
      INT();
      break;

    case BC_LREAD:
      PUSH(Lread());
      break;

    case BC_LWRITE:
      PUSH(Lwrite(POP()));
      break;

    case BC_LLENGTH:
      PUSH(Llength(POP_REF()));
      break;

    case BC_LSTRING:
      PUSH_REF(Lstring(POP_REF()));
      break;

    case BC_BARRAY: {
      size_t* arr = static_cast<size_t*>(Barray(INT()));
      PUSH_REF(arr);
      break;
    }

    case BC_PATT_STR: {
      size_t* scrut = POP_REF();
      size_t* str = POP_REF();
      PUSH(Bstring_patt(scrut, str));
      break;
    }

    default:
      switch (h) {

      case BC_STOP:
        goto stop;

      case BC_BINOP: {
        int a = UNBOX(POP());
        int b = UNBOX(POP());
        PUSH(BOX(binop(l - 1, b, a)));
        break;
      }

      case BC_LD:
        PUSH(*resolve_loc(cur_frame, l, INT()));
        break;

      case BC_LDA: {
        size_t* val_ptr = resolve_loc(cur_frame, l, INT());
        PUSH_REF(val_ptr);
        PUSH_REF(val_ptr);
        break;
      }

      case BC_ST:
        *resolve_loc(cur_frame, l, INT()) = TOP();
        break;

      case BC_PATT: {
        size_t* scrut = POP_REF();

        int (*PATT_FNS[]) (void*) = {
          Bstring_tag_patt,
          Barray_tag_patt,
          Bsexp_tag_patt,
          Bboxed_patt,
          Bunboxed_patt,
          Bclosure_tag_patt,
        };

        if (0 < l && l - 1 < sizeof(PATT_FNS) / sizeof(PATT_FNS[0])) {
          PUSH(PATT_FNS[l - 1](scrut));
        } else {
          failure("Unexpected pattern tag type\n");
        }
        break;
      }

      default:
        FAIL();
      }
    }
  }
  while (1);
  stop:

  return 0;
}

#endif
