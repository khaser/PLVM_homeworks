/* Lama SM Bytecode interpreter */
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>

#include "../runtime/gc.h"
#include "../runtime/runtime.h"
#include "runtime_externs.h"
#include "sm_encoding.h"

#define MAX_VSTACK_SZ      10000
#define MAX_FRAME_STACK_SZ 10000

extern size_t* __gc_stack_top; /* points to first uninitialized vstack element */
extern size_t* __gc_stack_bottom;

#define CHECK_UNDERFLOW() (__gc_stack_bottom - __gc_stack_top < 0 && (failure("vstack underflow\n"), 42))
#define CHECK_OVERFLOW() (__gc_stack_bottom - __gc_stack_top > MAX_VSTACK_SZ && (failure("vstack overflow\n"), 228))

#define PUSH(val) (CHECK_OVERFLOW(), *(__gc_stack_top--) = val)
#define PUSH_REF(val) PUSH((int) (val))
#define POP() (__gc_stack_top++, CHECK_UNDERFLOW(), *__gc_stack_top)
#define POP_REF() (int*) POP()
#define TOP() (*(__gc_stack_top+1))
#define ALLOC(n) do { __gc_stack_top -= (n); CHECK_OVERFLOW(); } while (0)
#define TRUNC(n) do { \
                      __gc_stack_top += (n); CHECK_UNDERFLOW();          \
                      memset(__gc_stack_top - (n - 1), 0, n * sizeof(size_t)); \
                 } while (0)

/* The unpacked representation of bytecode file */
typedef struct {
  char* string_ptr;              /* A pointer to the beginning of the string table */
  int*  public_ptr;              /* A pointer to the beginning of publics table    */
  char* code_ptr;                /* A pointer to the bytecode itself               */
  int   code_size;               /* The size (in bytes) of the bytecode section    */
  /* ^^^ Custom fields ^^^ */
  int   stringtab_size;          /* The size (in bytes) of the string table        */
  int   global_area_size;        /* The size (in words) of global area             */
  int   public_symbols_number;   /* The number of public symbols                   */
  char  buffer[0];
  /* ^^^ Bytefile structure ^^^ */
} bytefile;

/* Gets a string from a string table by an index */
static inline char* get_string(bytefile* f, int pos) {
  if (0 <= pos && pos < f->stringtab_size) {
    return &f->string_ptr[pos];
  } else {
    failure("String offset out of bounds.\nRequested string offset: %d\nStringtab size: %d\n", pos);
  }
}

/* Reads a binary bytecode file by name and unpacks it */
static bytefile* read_file(char* fname) {
  FILE* f = fopen(fname, "rb");
  long size;
  bytefile* file;

  if (f == 0) {
    failure("%s\n", strerror(errno));
  }

  if (fseek (f, 0, SEEK_END) == -1) {
    failure("%s\n", strerror(errno));
  }

  file = (bytefile*) malloc(sizeof(int*)*3 + sizeof(int) + (size = ftell (f)));

  if (file == 0) {
    failure("Unable to allocate memory.\n");
  }

  rewind(f);

  if (size != fread(&file->stringtab_size, 1, size, f)) {
    failure("%s\n", strerror(errno));
  }

  fclose(f);

  int stringtab_offset = file->public_symbols_number * 2 * sizeof(int);
  if (stringtab_offset < size) {
    file->string_ptr = file->buffer + file->public_symbols_number * 2 * sizeof(int);
  } else {
    failure("String table can't be found in file.\nRecognized string offset: %d,\nfile size: %d", \
            stringtab_offset, size);
  }
  file->public_ptr = (int*) file->buffer;
  int code_offset = stringtab_offset + file->stringtab_size;
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
    int*    vstack_base;
    size_t  locals;
    size_t  args;
    char    is_closure;
    void*   ret_ip;
} fstack[MAX_FRAME_STACK_SZ];

struct frame* const main_frame = fstack + MAX_FRAME_STACK_SZ - 1;

int vstack[MAX_VSTACK_SZ];

static int binop(char, int, int);
static int* resolve_loc(struct frame* cur_frame, char loc_type, int id);
static void* Barray(int n);
static void* Bsexp(int n, int tag);
static void* Bclosure(int n, void* entry);

int main (int argc, char* argv[]) {
  bytefile* bf = read_file(argv[1]);

  /* Init GC */
  __init();
  __gc_stack_bottom = vstack + MAX_VSTACK_SZ;
  __gc_stack_top = vstack + MAX_VSTACK_SZ - 1;

  ALLOC(bf->global_area_size);
  struct frame* cur_frame = main_frame;
  cur_frame->vstack_base = __gc_stack_top;

  char* ip     = bf->code_ptr;

#define CHECK_IP(x) (bf->code_ptr <= ip && ip + x <= bf->code_ptr + bf->code_size) || \
                      (failure("Instruction pointer outer of code bounds.\n" \
                               "IP: %p\nCode start: %p\nCode end: %p\n", \
                               ip, bf->code_ptr, bf->code_ptr + bf->code_size), 42)
#define INT()    (CHECK_IP(sizeof(int)), ip += sizeof (int), *(int*)(ip - sizeof (int)))
#define REF()    ((int*) INT())
#define BYTE()   (CHECK_IP(1), *ip++)
#define FAIL()   failure ("Invalid opcode %d-%d\n", h, l)
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
      PUSH_REF(Bstring(get_string(bf, INT())));
      break;
    }

    case BC_SEXP:
      char* tag = get_string(bf, INT());
      int sz = INT();
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
      ip = bf->code_ptr + INT();
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
      int dest = INT();
      if (1 ^ l ^ !!UNBOX(POP())) {
        ip = bf->code_ptr + dest;
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
      int args, dest;
      int* cls;

    case BC_CALLC:
      args = INT();
      cls = (int*) *(__gc_stack_top + 1 + args);
      goto call;

    case BC_CALL:
      dest = INT();
      args = INT();
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

      ip = bf->code_ptr + (cls ? *cls : dest);
      break;
    }

    case BC_TAG: {
      char* tag = get_string(bf, INT());
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

  return 0;
}

int binop(char opcode, int a, int b) {
    const int BINOP_COUNTER_START = __COUNTER__;
#define BINOP(op) case (__COUNTER__ - BINOP_COUNTER_START - 1): return a op b; break;
  switch (opcode) {
    BINOP(+);
    BINOP(-);
    BINOP(*);
    BINOP(/);
    BINOP(%);
    BINOP(<);
    BINOP(<=);
    BINOP(>);
    BINOP(>=);
    BINOP(==);
    BINOP(!=);
    BINOP(&&);
    BINOP(||);
    default:
      failure("Unsupported opcode for binary operation: %d\n", opcode);
  }
#undef BINOP
}


int* resolve_loc(struct frame* cur_frame, char loc_type, int idx) {
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

/* Following routines copy-pasted from runtime,
 * but takes arguments from local virtual stack instead of va_args (host stack) */
static void* Barray(int n) {
  int i;
  data* r= (data*) alloc_array(n);

  for (i = n - 1; i >= 0; --i) {
    ((int*)r->contents)[i] = POP();
  }

  return r->contents;
}

static void* Bsexp(int n, int tag) {
  int fields_cnt = n;
  data* r = (data *)alloc_sexp(fields_cnt);
  ((sexp *)r)->tag = 0;

  for (int i = n; i > 0; --i) {
    ((int *)r->contents)[i] = POP();
  }

  ((sexp *)r)->tag = tag;

  return (int *)r->contents;
}

static void* Bclosure(int n, void* entry) {
  data* r = (data*) alloc_closure(n + 1);
  ((void **)r->contents)[0] = entry;

  for (int i = n; i >= 1; --i) {
    ((int *)r->contents)[i] = POP();
  }

  return r->contents;
}
