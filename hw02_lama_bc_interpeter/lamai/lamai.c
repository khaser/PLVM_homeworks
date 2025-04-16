/* Lama SM Bytecode interpreter */
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>

#include "../runtime/gc.h"
#include "../runtime/runtime.h"
#include "runtime_externs.h"
#include "sm_encoding.h"
#include "vstack.h"

struct timespec getClock() {
   struct timespec tp;
   clock_gettime(CLOCK_MONOTONIC, &tp);
   return tp;
}

long long diffClock(const struct timespec a, const struct timespec b) {
   return (a.tv_sec - b.tv_sec) * 1e9 + (a.tv_nsec - b.tv_nsec);
}

/* The unpacked representation of bytecode file */
typedef struct {
  const char* string_ptr;         /* A pointer to the beginning of the string table */
  const int*  public_ptr;         /* A pointer to the beginning of publics table    */
  const char* code_ptr;           /* A pointer to the bytecode itself               */
  size_t      code_size;          /* The size (in bytes) of the bytecode section    */
  /* ^^^ Custom fields ^^^ */
  size_t stringtab_size;          /* The size (in bytes) of the string table        */
  size_t global_area_size;        /* The size (in words) of global area             */
  size_t public_symbols_number;   /* The number of public symbols                   */
  char   buffer[0];
  /* ^^^ Bytefile structure ^^^ */
} bytefile;

/* Gets a string from a string table by an index */
static inline const char* get_string(const bytefile* f, size_t pos) {
  if (pos < f->stringtab_size) {
    return &f->string_ptr[pos];
  } else {
    failure("String offset out of bounds.\nRequested string offset: %d\nStringtab size: %d\n", pos);
  }
}

/* Reads a binary bytecode file by name and unpacks it */
static inline bytefile* read_file(const char* fname) {
  FILE* f = fopen(fname, "rb");
  long size;
  bytefile* file;

  if (f == 0) {
    failure("%s\n", strerror(errno));
  }

  if (fseek (f, 0, SEEK_END) == -1) {
    failure("%s\n", strerror(errno));
  }

  file = (bytefile*) malloc(sizeof(int*) * 3 + sizeof(size_t) + (size = ftell(f)));

  if (file == 0) {
    failure("Unable to allocate memory.\n");
  }

  rewind(f);

  if (size != fread(&file->stringtab_size, 1, size, f)) {
    failure("%s\n", strerror(errno));
  }

  fclose(f);

  size_t stringtab_offset = file->public_symbols_number * 2 * sizeof(int);
  if (stringtab_offset < size) {
    file->string_ptr = file->buffer + file->public_symbols_number * 2 * sizeof(int);
  } else {
    failure("String table can't be found in file.\nRecognized string offset: %d,\nfile size: %d", \
            stringtab_offset, size);
  }
  file->public_ptr = (int*) file->buffer;
  size_t code_offset = stringtab_offset + file->stringtab_size;
  if (code_offset < size) {
    file->code_ptr = file->buffer + code_offset;
    file->code_size = size - code_offset + 1;
  } else {
    failure("Code section can't be found in file.\nRecognized code offset: %d,\nfile size: %d", \
            code_offset, size);
  }

  return file;
}

struct frame {
    int*        vstack_base;
    size_t      locals;
    size_t      args;
    char        is_closure;
    const void* ret_ip;
} fstack[MAX_FRAME_STACK_SZ];

struct frame* const main_frame = fstack + MAX_FRAME_STACK_SZ - 1;

int vstack[MAX_VSTACK_SZ];

static inline int binop(char opcode, int a, int b) {
    switch (opcode) {
#define DEF(opcode, op) case opcode: return a op b; break;
    BINOPS(DEF)
#undef DEF
    default:
      failure("Unsupported opcode for binary operation: %d\n", opcode);
  }
}

static inline __attribute__((always_inline)) int* resolve_loc(const struct frame* cur_frame, char loc_type, int idx) {
  switch (loc_type) {
    case GLOBAL:
      return __gc_stack_bottom - 1 - idx;
    case LOCAL:
      return cur_frame->vstack_base - idx;
    case ARGUMENT:
      return cur_frame->args + cur_frame->vstack_base - idx;
    case CAPTURED:
      int* cls = *(int**)(cur_frame->args + cur_frame->vstack_base + 1);
      return cls + 1 + idx;
    default:
      failure("Unsupported memory operation operand type: %d\n", loc_type);
  }
}

int main (int argc, char* argv[]) {
  bytefile* bf = read_file(argv[1]);

  struct timespec startup = getClock();

  /* Init GC */
  __init();
  __gc_stack_bottom = vstack + MAX_VSTACK_SZ;
  __gc_stack_top = vstack + MAX_VSTACK_SZ - 1;

  ALLOC(bf->global_area_size);
  struct frame* cur_frame = main_frame;
  cur_frame->vstack_base = __gc_stack_top;

  const char* ip = bf->code_ptr;

#define CHECK_IP(ip, sz) (bf->code_ptr <= (ip) && (ip) + (sz) <= bf->code_ptr + bf->code_size)
#define ASSERT_IP(sz) (CHECK_IP(ip, sz) || \
                      (failure("Instruction pointer outer of code bounds.\n" \
                               "IP: %p\nCode start: %p\nCode end: %p\n", \
                               ip, bf->code_ptr, bf->code_ptr + bf->code_size), 42))
#define GENERIC_IP_PEEK(type) (ASSERT_IP(sizeof(type)), ip += sizeof(type), *(type*)(ip - sizeof(type)))
#define INT()    (GENERIC_IP_PEEK(int))
#define UINT()   (GENERIC_IP_PEEK(size_t))
#define REF()    (GENERIC_IP_PEEK(int*))
#define BYTE()   (GENERIC_IP_PEEK(unsigned char))
#define FAIL()   failure ("Invalid opcode %d-%d\n", h, l)

#define MAKE_JUMP(offset) \
    do { \
      if (!CHECK_IP(bf->code_ptr + (offset), 1)) { \
        failure("Incorrect jump destination.\n" \
                "Jump destination: %x\nCurrent IP: %x\n", \
                offset, ip - bf->code_ptr); \
      } \
      ip = bf->code_ptr + (offset); \
    } while (0);

  do {
    unsigned char x = BYTE(),
         h = (x & 0xF0) >> 4,
         l = x & 0x0F;

    switch (x) {
    case BC_CONST:
      int val = INT();
      PUSH(BOX(val));
      break;

    case BC_STRING: {
      PUSH_REF(Bstring(get_string(bf, UINT())));
      break;
    }

    case BC_SEXP:
      const char* tag = get_string(bf, UINT());
      size_t sz = UINT();
      int* bsexp = Bsexp(sz, UNBOX(LtagHash(tag)));
      PUSH_REF(bsexp);
      break;

    case BC_STI: {
      int val = POP();
      int* dest = POP_REF();
      PUSH(*dest = val);
      break;
    }

    case BC_STA: {
      int* arr = POP_REF();
      int idx = POP();
      int* val_ptr = POP_REF();
      PUSH_REF(Bsta(arr, idx, val_ptr));
      break;
    }

    case BC_JMP:
      size_t offset = UINT();
      MAKE_JUMP(offset);
      break;

    case BC_END:
    case BC_RET: {
      int ret = POP();
      TRUNC(cur_frame->locals + cur_frame->args + cur_frame->is_closure);
      PUSH(ret);
      ip = cur_frame->ret_ip;
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

    case BC_SWAP:
      int a = POP();
      int b = POP();
      PUSH(b);
      PUSH(a);
      break;

    case BC_ELEM: {
      int idx = POP();
      int* arr = POP_REF();
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
      int* cls = Bclosure(args, dest);
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
      const char* tag = get_string(bf, UINT());
      int args = INT();
      int* sexp = POP_REF();
      PUSH(Btag(sexp, LtagHash(tag), BOX(args)));
      break;
    }

    case BC_ARRAY: {
      int sz = INT();
      int* arr = POP_REF();
      PUSH(Barray_patt(arr, BOX(sz)));
      break;
    }

    case BC_FAIL: {
      int line = INT();
      int col = INT();
      Bmatch_failure(POP_REF(), argv[1], line, col);
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

    case BC_BARRAY:
      int* arr = Barray(INT());
      PUSH_REF(arr);
      break;

    case BC_PATT_STR: {
      int* scrut = POP_REF();
      int* str = POP_REF();
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
        int* val_ptr = resolve_loc(cur_frame, l, INT());
        PUSH_REF(val_ptr);
        PUSH_REF(val_ptr);
        break;
      }

      case BC_ST:
        *resolve_loc(cur_frame, l, INT()) = TOP();
        break;

      case BC_PATT: {
        int* scrut = POP_REF();
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

  long long time_ns = diffClock(getClock(), startup);
  printf("Interpretation time:\t%lld[ns]\n", time_ns);

  return 0;
}
