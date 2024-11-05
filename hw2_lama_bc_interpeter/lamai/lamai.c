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

void* __start_custom_data;
void* __stop_custom_data;

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
#define POP() (*(++__gc_stack_top))
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

int main (int argc, char* argv[]) {
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
#define BYTE   *ip++
#define FAIL   failure ("ERROR: invalid opcode %d-%d\n", h, l)
  do {
    // fprintf(stderr, "%p\n", ip);
    char x = BYTE,
         h = (x & 0xF0) >> 4,
         l = x & 0x0F;


    switch (h) {
    case 15:
      goto stop;

    case 0:
      /* BINOP */
      int a = UNBOX(POP());
      int b = UNBOX(POP());
      PUSH(BOX(binop(l - 1, b, a)));
      break;

    case 1:
      switch (l) {
      case  0:
        /* CONST */
        PUSH(BOX(INT));
        break;

      case  1:
        // fprintf (f, "STRING\t%s", STRING);
        break;

      case  2:
        // fprintf (f, "SEXP\t%s ", STRING);
        // fprintf (f, "%d", INT);
        break;

      case  3:
        // fprintf (f, "STI");
        break;

      case  4:
        // fprintf (f, "STA");
        break;

      case  5:
        /* JMP */
        ip = bf->code_ptr + INT;
        break;

      case  6:
        // fprintf (f, "END");
        break;

      case  7:
        // fprintf (f, "RET");
        break;

      case  8:
        /* DROP */
        POP();
        break;

      case  9:
        // fprintf (f, "DUP");
        break;

      case 10:
        // fprintf (f, "SWAP");
        break;

      case 11:
        // fprintf (f, "ELEM");
        break;

      default:
        FAIL;
      }
      break;

    case 2:
      /* LD */
      PUSH(*resolve_loc(l, INT));
      break;
    case 3:
      /* LDA */
      break;
    case 4:
      /* ST */
      *resolve_loc(l, INT) = TOP();
      break;
    case 5:
      switch (l) {
      case  0:
      case  1:
        /* CJMPz/CJMPnz */
        int dest = INT;
        if (1 ^ l ^ !!UNBOX(POP())) {
          ip = bf->code_ptr + dest;
        }
        break;

      case  2:
        /* BEGIN */
        int args = INT;
        int locals = INT;
        ALLOC(locals);
        cur_frame->locals = locals;
        break;

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

      case  6:
        // fprintf (f, "CALL\t0x%.8x ", INT);
        // fprintf (f, "%d", INT);
        break;

      case  7:
        // fprintf (f, "TAG\t%s ", STRING);
        // fprintf (f, "%d", INT);
        break;

      case  8:
        // fprintf (f, "ARRAY\t%d", INT);
        break;

      case  9:
        // fprintf (f, "FAIL\t%d", INT);
        // fprintf (f, "%d", INT);
        break;

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
        /* READ */
        PUSH(Lread());
        break;

      case 1:
        /* WRITE */
        PUSH(Lwrite(POP()));
        break;

      case 2:
        // fprintf (f, "CALL\tLlength");
        break;

      case 3:
        // fprintf (f, "CALL\tLstring");
        break;

      case 4:
        // fprintf (f, "CALL\tBarray\t%d", INT);
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
