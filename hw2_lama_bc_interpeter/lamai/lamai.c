/* Lama SM Bytecode interpreter */
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <malloc.h>

#include "../runtime/gc.h"
#include "../runtime/runtime.h"
#include "runtime_externs.h"

#define MAX_VSTACK_SZ      10000
#define MAX_FRAME_STACK_SZ 1000

extern size_t* __gc_stack_top; // points to first uninitialized vstack element
extern size_t* __gc_stack_bottom;

void check_vstack() {
  size_t vstack_size = __gc_stack_bottom - __gc_stack_top;
  if (vstack_size < 0) {
    failure("vstack underflow");
  }
  if (vstack_size > MAX_VSTACK_SZ) {
    failure("vstack overflow");
  }
}

#define PUSH(val) (*(__gc_stack_top--) = val)
#define PUSH_REF(val) (PUSH((int) (val)))
#define POP() (*(++__gc_stack_top))
#define POP_REF() ((int*) POP())
#define TOP() (*(__gc_stack_top+1))
#define ALLOC(n) (__gc_stack_top -= (n))
#define TRUNK(n) (__gc_stack_top += (n))

/* The unpacked representation of bytecode file */
typedef struct {
  char* string_ptr;              /* A pointer to the beginning of the string table */
  int*  public_ptr;              /* A pointer to the beginning of publics table    */
  char* code_ptr;                /* A pointer to the bytecode itself               */
  int*  global_ptr;              /* A pointer to the global area                   */
  int   stringtab_size;          /* The size (in bytes) of the string table        */
  int   global_area_size;        /* The size (in words) of global area             */
  int   public_symbols_number;   /* The number of public symbols                   */
  char  buffer[0];
} bytefile;

/* Gets a string from a string table by an index */
char* get_string (bytefile *f, int pos) {
  return &f->string_ptr[pos];
}

/* Reads a binary bytecode file by name and unpacks it */
bytefile* read_file (char *fname) {
  FILE *f = fopen (fname, "rb");
  long size;
  bytefile *file;

  if (f == 0) {
    failure ("%s\n", strerror (errno));
  }

  if (fseek (f, 0, SEEK_END) == -1) {
    failure ("%s\n", strerror (errno));
  }

  file = (bytefile*) malloc (sizeof(int*)*4 + (size = ftell (f)));

  if (file == 0) {
    failure ("*** FAILURE: unable to allocate memory.\n");
  }

  rewind (f);

  if (size != fread (&file->stringtab_size, 1, size, f)) {
    failure ("%s\n", strerror (errno));
  }

  fclose (f);

  file->string_ptr  = &file->buffer [file->public_symbols_number * 2 * sizeof(int)];
  file->public_ptr  = (int*) file->buffer;
  file->code_ptr    = &file->string_ptr [file->stringtab_size];
  file->global_ptr  = (int*) malloc (file->global_area_size * sizeof (int));

  return file;
}

struct frame {
    int*    vstack_base;
    size_t  locals;
    size_t  args;
    char    is_closure;
    void*   ret_ip;
} fstack[MAX_FRAME_STACK_SZ];

struct frame* cur_frame = fstack + MAX_FRAME_STACK_SZ - 1;

int vstack[MAX_VSTACK_SZ];

int binop(char, int, int);
int* resolve_loc(char loc_type, int id);
static void *Barray (int n);

int main (int argc, char* argv[]) {
  // TODO: usage
  bytefile* bf = read_file(argv[1]);

  /* Init GC */
  __init();
  __gc_stack_bottom = vstack + MAX_VSTACK_SZ;
  __gc_stack_top = vstack + MAX_VSTACK_SZ - 1;

  ALLOC(bf->global_area_size);
  cur_frame->vstack_base = __gc_stack_top;

  char* ip     = bf->code_ptr;
  char* pats[] = {"=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun"};
  char* lds [] = {"LD", "LDA", "ST"};

#define INT    (ip += sizeof (int), *(int*)(ip - sizeof (int)))
#define REF    ((int*) INT)
#define BYTE   *ip++
#define FAIL   failure ("ERROR: invalid opcode %d-%d\n", h, l)
  do {
    char x = BYTE,
         h = (x & 0xF0) >> 4,
         l = x & 0x0F;

    fprintf(stderr, "%p, %d, %d\n", ip - bf->code_ptr - 1, h, l);

    switch (h) {
    case 15:
      goto stop;

    case 0: {
      /* BINOP */
      int a = UNBOX(POP());
      int b = UNBOX(POP());
      PUSH(BOX(binop(l - 1, b, a)));
      break;
    }

    case 1:
      switch (l) {
      case  0:
        /* CONST */
        PUSH(BOX(INT));
        break;

      case  1: {
        /* STRING */
        PUSH_REF(Bstring(get_string(bf, INT)));
        break;
      }

      case  2:
        // fprintf (f, "SEXP\t%s ", STRING);
        // fprintf (f, "%d", INT);
        break;

      case  3: {
        /* STI */
        int val = POP();
        int* dest = POP_REF();
        PUSH(*dest = val);
        break;
      }

      case  4: {
        /* STA */
        int* arr = POP_REF();
        int idx = POP();
        int* val_ptr = POP_REF();
        PUSH_REF(Bsta(arr, idx, val_ptr));
        break;
      }

      case  5:
        /* JMP */
        ip = bf->code_ptr + INT;
        break;

      case  6:
      case  7: {
        /* END, RET */
        int ret = POP();
        TRUNK(cur_frame->locals + cur_frame->args);
        PUSH(ret);
        ip = cur_frame->ret_ip;
        cur_frame++;
        // TODO: detect frame underflow
        break;
      }

      case  8:
        /* DROP */
        POP();
        break;

      case  9:
        /* DUP */
        PUSH(TOP());
        break;

      case 10:
        // fprintf (f, "SWAP");
        break;

      case 11: {
        /* ELEM */
        int idx = POP();
        int* arr = POP_REF();
        PUSH_REF(Belem(arr, idx));
        break;
      }

      default:
        FAIL;
      }
      break;

    case 2:
      /* LD */
      PUSH(*resolve_loc(l, INT));
      break;
    case 3: {
      /* LDA */
      int* val_ptr = resolve_loc(l, INT);
      PUSH_REF(val_ptr);
      PUSH_REF(val_ptr);
      break;
    }
    case 4:
      /* ST */
      *resolve_loc(l, INT) = TOP();
      break;
    case 5:
      switch (l) {
      case  0:
      case  1: {
        /* CJMPz/CJMPnz */
        int dest = INT;
        if (1 ^ l ^ !!UNBOX(POP())) {
          ip = bf->code_ptr + dest;
        }
        break;
      }

      case  2: {
        /* BEGIN */
        int args = INT;
        int locals = INT;
        ALLOC(locals);
        cur_frame->locals = locals;
        break;
      }

      case  3:
        // fprintf (f, "CBEGIN\t%d ", INT);
        // fprintf (f, "%d", INT);
        break;

      case  4:
        // fprintf (f, "CLOSURE\t0x%.8x", INT);
        {int n = INT;
         for (int i = 0; i<n; i++) {
         switch (BYTE) {
           // case 0: fprintf (f, "G(%d)", INT); break;
           // case 1: fprintf (f, "L(%d)", INT); break;
           // case 2: fprintf (f, "A(%d)", INT); break;
           // case 3: fprintf (f, "C(%d)", INT); break;
           default: FAIL;
         }
         }
        };
        break;

      case  5:
        // fprintf (f, "CALLC\t%d", INT);
        break;

      case  6: {
        /* CALL */
        int dest = INT;
        int args = INT;

        /* reverse call arguments on stack */
        int* rev_l = __gc_stack_top + 1;
        int* rev_r = __gc_stack_top + 1 + args;
        for (; rev_l < rev_r; ++rev_l, --rev_r) {
          /* swap trick */
          *rev_l = *rev_l ^ *rev_r;
          *rev_r = *rev_l ^ *rev_r;
          *rev_l = *rev_l ^ *rev_r;
        }

        cur_frame--;
        cur_frame->ret_ip = ip;
        cur_frame->vstack_base = __gc_stack_top;
        cur_frame->args = args;
        ALLOC(args);

        ip = bf->code_ptr + dest;
        break;
      }

      case  7: {
        /* TAG */
        char* tag = get_string(bf, INT);
        int args = BOX(INT);
        int* sexp = POP_REF();
        PUSH(Btag(sexp, LtagHash(tag), args));
        break;
      }

      case  8: {
        /* ARRAY */
        int sz = BOX(INT);
        int* arr = POP_REF();
        PUSH(Barray_patt(arr, sz));
        break;
      }

      case  9: {
        /* FAIL */
        int line = INT;
        int col = INT;
        Bmatch_failure(POP_REF(), argv[1], line, col);
        break;
      }

      case 10:
        /* LINE */
        INT;
        break;

      default:
        FAIL;
      }
      break;

    case 6:
      // fprintf (f, "PATT\t%s", pats[l]);
      break;

    case 7: {
      switch (l) {
      case 0:
        /* Lread */
        PUSH(Lread());
        break;

      case 1:
        /* Lwrite */
        PUSH(Lwrite(POP()));
        break;

      case 2:
        /* Llength */
        PUSH(Llength(POP_REF()));
        break;

      case 3:
        /* Lstring */
        PUSH_REF(Lstring(POP_REF()));
        break;

      case 4:
        int sz = BOX(INT);
        PUSH_REF(Barray(sz));
        break;

      default:
        FAIL;
      }
    }
    break;

    default:
      FAIL;
    }

    check_vstack();
  }
  while (1);
  stop:

  return 0;
}

int binop(char opcode, int a, int b) {
  if (opcode == 0) {
    return a + b;
  } else if (opcode == 1)  {
    return a - b;
  } else if (opcode == 2)  {
    return a * b;
  } else if (opcode == 3)  {
    return a / b;
  } else if (opcode == 4)  {
    return a % b;
  } else if (opcode == 5)  {
    return a < b;
  } else if (opcode == 6)  {
    return a <= b;
  } else if (opcode == 7)  {
    return a > b;
  } else if (opcode == 8)  {
    return a >= b;
  } else if (opcode == 9)  {
    return a == b;
  } else if (opcode == 10) {
    return a != b;
  } else if (opcode == 11) {
    return a && b;
  } else if (opcode == 12) {
    return a || b;
  } else {
    failure("Unsupported opcode for binary operation: %d\n", opcode);
  }
}


int* resolve_loc(char loc_type, int id) {
  if (loc_type == 0) {
    /* GLOBAL */
    return __gc_stack_bottom - 1 - id;
  } else if (loc_type == 1) {
    /* LOCAL */
    return cur_frame->vstack_base - id;
  } else if (loc_type == 2) {
    /* ARGUMENT */
    return cur_frame->vstack_base + 1 + id;
  } else if (loc_type == 3) {
    /* CAPTURED */
    failure("TODO\n");
  } else {
    failure("Unsupported memory operation operand type: %d\n", loc_type);
  }
}

/* Copy-pasted from runtime, but migrated into local virtual stack */
static void *Barray (int n) {
  int     i, ai;
  data   *r;

  r = (data *)alloc_array(n);

  for (i = n - 1; i >= 0; --i) {
    ai                      = POP();
    ((int *)r->contents)[i] = ai;
  }

  return r->contents;
}

// TODO: other structures operations
